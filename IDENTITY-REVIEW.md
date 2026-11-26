# Identity review

## Stato attuale: identità live UXP

La configurazione predefinita usa lo ZAPPLET scritto nel progetto (sorgente
ASM e binario in `c/applets/identity/`): legge **solo modello e versione
firmware** dal banner monitor, tramite il protocollo privato LTID/LTRS. Il
seriale non viene richiesto e non compare nella UI. Un banner di fallback
`LabTool-UNKNOWN Firmware Version 0.00` viene rifiutato, non mostrato come
identità reale. Per tornare al percorso originale di sola lettura U62 si
configura `-DORIGINAL_IDENTITY_APPLET=ON`.

`lt48_programmer_describe()` resta una fotografia dei descrittori USB, ma non
fornisce più un modello inventato quando `iProduct` è assente.
`lt48_programmer_identify()` opera sul lettore già aperto, senza usare
`bcdDevice` come versione firmware:

- default: ZAPPLET chiama internamente il comando monitor `51`, ottenendo
  il banner dal programmatore, ad esempio
  `LabTool-48UXP Firmware Version 4.10`; modello e versione sono estratti dal
  testo ricevuto, non derivati dal PID;
- opt-in `ORIGINAL_IDENTITY_APPLET`: comando monitor `51 AA 55` dal host
  e applet originale di sola lettura `7e1df8`; i suoi 13 byte vengono
  decodificati per ottenere il seriale U62, ad esempio `AE0605100067`.

L'applet (600 byte, SHA-256 `aaca6cb58adbbfa66e5b42a78c3389f4b14b92717b55f333942a06b38d8e942c`)
non è distribuito nel repository: `build-lt48uxp install`, `postinst` o
`identity` lo estraggono dall'EXE originale verificandone l'hash. Nel default
non viene usato: la GUI mostra il solo modello e firmware reali e il report
schema 4 lascia il seriale vuoto. Nella modalità opt-in, se l'applet originale
manca, compare **seriale non disponibile**; non si usa mai `iSerialNumber`
come sostituto.

Il contratto di capability per lettori classici/SPP e adattatori resta da
progettare: l'identity live qui è specifica del monitor UXP, non prova che un
PID identifichi univocamente una famiglia di lettori.

## Situazione precedente: solo descrittori USB

Il software ha un livello identity, ma è **solo descrittore USB**:

- modello: `iProduct`, con fallback fisso `LabTool-48UXP`;
- seriale: `iSerialNumber`, può essere vuoto;
- versione: `bcdDevice`, ma è la release USB del dispositivo, **non la ROM
  monitor**;
- ruolo: derivato dal solo `idProduct` (`2003` ready, `2004` cold, `2005`
  alternate).

Sul dispositivo reale:

```text
idVendor     11b5
idProduct    2004
bcdDevice    0.00
iManufacturer 0
iProduct      0
iSerial       0
```

Quindi seriale e modello reale **non ci sono**. Il modello viene sostituito
dal fallback; il seriale resta vuoto.

## Dove sta il codice

- `c/src/usb_libusb.c:158`: lettura dei descrittori;
- `c/src/usb_libusb.c:172`: `iProduct` -> `model`;
- `c/src/usb_libusb.c:175`: `iSerialNumber` -> `serial_number`;
- `c/src/usb_libusb.c:177`: fallback modello;
- `c/src/usb_libusb.c:188`: `lt48_programmer_describe`;
- `c/src/core.c:61`: ruolo da `idProduct`;
- `c/src/usb_libusb.c:493/529/563`: `2004` trattato come cold e usato per il
  caricamento FX2.

## Cosa mancava prima del fix

- nessuna lettura del banner monitor `LabTool-48... Firmware Version`;
- nessun comando monitor per la versione ROM;
- nessun uso di `3FF0`, `3FE0`, `6B`, `6C` per identità;
- nessuna capability per modello, ROM, seriale o trasporto;
- nessuna dispatch del comportamento in base a modello/seriale.

`model` e `serial` sono usati per UI e report. Il comportamento operativo è
scelto da database, definizione dispositivo e PID USB.

## Perché è insufficiente per lettori diversi

Per supportare 2.2 SPP, 4.x EPP e futuri lettori serve un’identità esplicita,
non stringhe descrittore:

```c
enum lt48_reader_kind {
    LT48_READER_UNKNOWN,
    LT48_READER_48_CLASSIC,
    LT48_READER_48XP,
    LT48_READER_48UXP
};

struct lt48_reader_caps {
    uint32_t flags;        /* SPP, EPP, 3V3, VPP, applet, security */
    uint16_t rom_version;
    uint16_t protocol_version;
};
```

Contratto proposto:

- probe composto dopo open/claim: banner monitor sul bulk bridge e,
  quando disponibile, applet originale di sola lettura U62;
- ritorna modello e firmware monitor verificati, seriale con stato separato;
- il seriale resta descrittivo, non usato per dispatch;
- `lt48_programmer_describe` resta descriptor-only;
- `lt48_programmer_identify()` per UXP è disponibile dopo open/claim;
- il dispatch `kind`/`caps` per altri trasporti resta una fase successiva;
- dispatch su `kind` + `caps`, fallback esplicito su `UNKNOWN`.

Per un adattatore USB del 2.2:

```text
kind              LT48_READER_48_CLASSIC
protocol          SPP (ed EPP se implementato)
caps              SPP | VPP5 | applet | security
rom_version       2.20
```

## Nota PID

Se l’adattatore riusa `11b5:2004`, viene classificato cold e può innescare il
caricamento FX2. Per un adattatore autonomo conviene:

- usare un PID READY/READY_ALT dedicato, oppure
- estendere `lt48_is_supported_usb_id` e il ruolo con un PID adapter.

## Conclusione

L’identity attuale basta per mostrare un nome e un seriale se il descrittore li
fornisce, ma non per modulare lettori diversi. Serve un contratto identity
esplicito; il PID può selezionare la famiglia solo in modo grossolano.
