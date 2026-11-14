# FO4 Map Marker Swap (WIP)

Drop PNGs in `Data/Interface/MapMarkers/` to replace Pip-Boy map markers. Icons keep their own colours (no UI tint).

    types/<Type>.png          replaces every marker of that type (SettlementMarker -> Settlement.png)
    locations/<EditorID>.png  replaces one place (or 0x<FormID>.png)   [needs runtime check]
    mapping.csv               default blip assignments (52 types pre-mapped, rest stay vanilla)

Resolution: location file -> type file -> vanilla.

## Status
- DONE + tested: folder layout, default mapping (mapping.csv; icons not included), `src/IconResolver.h` (compiles, resolves correctly).
- NOT DONE: F4SE glue (needs F4SE/CommonLibF4 + Windows build) and a tiny AS3 loader SWF.
  Plan, mirroring PipboyTabs.dll: on PipboyMenu open, add a loader child to `Menu_mc`, wait for
  Pipboy_MapPage.swf, then for each clip from `MarkerTypesA`/`WorldMapMarkers`/`LocalMapMarkers`
  hide the vanilla symbol, add a Bitmap from the resolved PNG, and skip the ColorTransform.
- All marker type names are in Pipboy_MapPage.swf (~90) and MapMarkers.swf (51, Discovered/Undiscovered frames).
