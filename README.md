# ibus-mudule

This software is provided "as is" without express or implied warranty. Any use is at your own risk.

# About
IBus module is HW module which provide bellow addition feature to BMW vehicles:
 - implements new menu on MID. This menu can be used for set all bellow features.
 - display distance from park distance module (PDC)
 - show welcome or reminder message on IKE display
 - lock doors when vehicle reach some speed. Speed can be adjusted by menu.
 - auto mirror fold/unfold.
 - welcome lights
 - CD changer emulation (not tested)

Bellow video shows most of the features:
https://www.youtube.com/watch?v=QW1V_MZT6Wc

Some of features depend on car equipment (PDC, IKE, MID, folding mirrors).

# Hardware
This software runs on custom hardware. It could not be run on any of original car modules. Software is test on Arduino Mini Pro. Apart of that some IBUS to UART converter is needed. This software is tested only on BMW E39 equipped with IKE, MID, rear PDC and folding mirrors. But it is applicable to BMW E38, E39, E46 and E53. This software supports only MID. Non of the other multimedia equipments are supported yet.

# Important: You need feedback between UART Tx and Rx because this software support IBUS collision detection.

# Known issues:
- writing on MID sometimes fails, but success rate is more than 95%.
- mirror folding does not work stable, because diagnostic commands are used. Success rate is about 90%
- remote key on button press command are not always send over IBUS. Because of that mirror fold on lock button hold is removed.
