#!/usr/bin/env python3
"""Build project-only Ubuntu .deb and extract-and-run tarball (requires dpkg-dev)."""
import hashlib,json,os,shutil,subprocess,tarfile,tempfile,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
VERSION='0.1.0-test.20261124.1'
DEB_VERSION='0.1.0~test.20261124.1'
build=Path(os.environ.get('LT48_BUILD_DIR',ROOT/'build'))
out=Path(os.environ.get('LT48_DIST_DIR',ROOT/'dist'));out.mkdir(parents=True,exist_ok=True)
def run(*args,**kw):return subprocess.run([str(x) for x in args],check=True,**kw)
run(sys.executable, ROOT/'packaging/check-source.py', '--root', ROOT)
run('cmake','-S',ROOT,'-B',build,'-DCMAKE_BUILD_TYPE=Release','-DCMAKE_INSTALL_PREFIX=/usr','-DCMAKE_INSTALL_LIBDIR=lib')
run('cmake','--build',build,'-j2')
run('ctest','--test-dir',build,'--output-on-failure')
with tempfile.TemporaryDirectory(prefix='lt48-pack-') as temp:
 temp=Path(temp);stage=temp/'stage'
 run('cmake','--install',build,'--strip',env={**os.environ,'DESTDIR':str(stage)})
 usr=stage/'usr'
 run(usr/'bin/labtool48','version','--json')
 run(usr/'bin/labtool48','new-device-lib','validate',usr/'share/labtool48/NewDeviceLib','--json')
 # Strict project-only install surface. A single authored applet has exact provenance.
 for p in stage.rglob('*'):
  if p.is_symlink():
   assert not Path(os.readlink(p)).is_absolute(),p
   continue
  if not p.is_file():continue
  rel=p.relative_to(stage).as_posix()
  assert rel.startswith(('usr/bin/','usr/lib/','usr/include/labtool48/','usr/share/applications/','usr/share/labtool48/NewDeviceLib/')),rel
  assert p.suffix.lower() not in ('.rez','.exe','.hex','.dll','.sys','.cab','.bit','.mcs','.jed','.svf','.xsvf'),rel
  if p.suffix=='.bin':assert rel=='usr/share/labtool48/NewDeviceLib/applets/atmel-word-timeout-tail.bin',rel
 # Let Debian derive ABI requirements from all three ELF objects.
 (temp/'debian').mkdir();(temp/'debian/control').write_text('Source: lt48uxp\n\nPackage: lt48uxp\nArchitecture: any\nDescription: Independent programmer application\n')
 dep=subprocess.check_output(['dpkg-shlibdeps','-O','--ignore-missing-info','-l'+str(usr/'lib'),'-e'+str(usr/'bin/labtool48'),'-e'+str(usr/'bin/labtool48-gtk'),'-e'+str(usr/'lib/liblabtool48.so.0.1.0')],cwd=temp,text=True)
 depends=next(x.split('=',1)[1] for x in dep.splitlines() if x.startswith('shlibs:Depends='))
 arch=subprocess.check_output(['dpkg','--print-architecture'],text=True).strip()
 control=stage/'DEBIAN';control.mkdir()
 (control/'control').write_text(f'Package: lt48uxp\nVersion: {DEB_VERSION}\nSection: electronics\nPriority: optional\nArchitecture: {arch}\nMaintainer: ashurbanipal-labs <noreply@users.noreply.github.com>\nDepends: {depends}\nInstalled-Size: {sum(p.stat().st_size for p in usr.rglob("*") if p.is_file())//1024}\nDescription: Independent LabTool-48UXP C and GTK application\n Vendor databases and USB firmware must be supplied separately.\n')
 for name,body in [('postinst','if command -v ldconfig >/dev/null; then ldconfig; fi\nif command -v udevadm >/dev/null; then udevadm control --reload-rules || true; fi\n'),('postrm','if command -v ldconfig >/dev/null; then ldconfig; fi\nif command -v udevadm >/dev/null; then udevadm control --reload-rules || true; fi\n')]:
  p=control/name;p.write_text('#!/bin/sh\nset -e\n'+body+'exit 0\n');p.chmod(0o755)
 deb=out/f'lt48uxp_{VERSION}_{arch}.deb'
 run('dpkg-deb','--root-owner-group','--build',stage,deb)
 portable=temp/'lt48uxp';shutil.copytree(usr,portable,symlinks=True)
 for name,binary in [('start-lt48uxp','labtool48-gtk'),('lt48-cli','labtool48')]:
  p=portable/name;p.write_text('#!/bin/sh\nset -eu\nHERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)\nexport LT48_NEW_DEVICE_LIB="$HERE/share/labtool48/NewDeviceLib"\nexport LD_LIBRARY_PATH="$HERE/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"\nexec "$HERE/bin/'+binary+'" "$@"\n');p.chmod(0o755)
 for p in ('README.md','BUILD-INFO.json','VERSION_HISTORY.md'):
  shutil.copy2(ROOT/p,portable/p)
 shutil.copytree(ROOT/'docs',portable/'docs')
 run(portable/'lt48-cli','version','--json')
 archive=out/f'lt48uxp-{VERSION}-ubuntu26.04-{arch}.tar.gz'
 with tarfile.open(archive,'w:gz') as t:t.add(portable,arcname='lt48uxp')
 (out/'PACKAGE-CHECKS.json').write_text(json.dumps({'build':VERSION,'depends':depends,'checks':['source build','offline diagnostics and buffer tests','staged CLI version','staged NewDeviceLib manifest validation','project-only install surface','portable CLI launcher'],'hardware_operations':False},indent=2)+'\n')
 print('Packages:',deb,archive)
