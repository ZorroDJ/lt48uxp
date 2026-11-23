# Exact Atmel byte-NOR VID defaults

Six Dataman 13.10 definitions D6a/D6c/D76/D7b/D7d/D84 use missing logical
pin zero in RESET W5 and GETID W58. Source lookup selects engine pin zero,
UDN ZIF1, outside the selected 40/44-pin package. VID uses channel 3 at 12 V.
GETID enables and then disables it; reset disables it in protection modes.
The guard checks every default offset, opcode, logical pin and classification.
The loader also binds the full source REZ digest and exact profile closure.
No generic missing-pin allowance is introduced.

Evidence: analysis/xp-shared-byte75-batch/gaps.json and
 tools/audit_xp_shared_byte75_gaps.py. Hardware testing is pending.
