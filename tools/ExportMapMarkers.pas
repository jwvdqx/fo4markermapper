{
  Export every map marker in the loaded plugins to a CSV.
  FO4Edit: load Fallout4.esm + all DLC (+ mods), select everything (Ctrl+A in the left tree),
  right-click -> Apply Script -> this file. Output: <FO4Edit folder>\Edit Scripts\MapMarkers.csv
  Columns: file;formid;editorid;name;type;cell;location;x;y
  UNTESTED: element paths follow the FO4 REFR layout; if a column is empty, send me one marker's
  full record (right-click a REFR with a Map Marker -> Copy as ... or a screenshot) and I'll adjust.
}
unit ExportMapMarkers;

var
  sl: TStringList;

function Initialize: integer;
begin
  sl := TStringList.Create;
  sl.Add('file;formid;editorid;name;type;cell;location;x;y');
  Result := 0;
end;

function Process(e: IInterface): integer;
var
  mm, cell, loc: IInterface;
  nm, tp, cn, ln, px, py: string;
begin
  Result := 0;
  if Signature(e) <> 'REFR' then Exit;
  mm := ElementByPath(e, 'Map Marker');
  if not Assigned(mm) then Exit;

  nm := GetElementEditValues(e, 'Map Marker\FULL - Name');
  tp := GetElementEditValues(e, 'Map Marker\TNAM - Marker Data\Type');
  if tp = '' then tp := GetElementEditValues(e, 'Map Marker\TNAM\Type');

  cn := ''; ln := '';
  cell := GetContainer(e); // walk up the group tree to the owning CELL
  while Assigned(cell) and (Signature(cell) <> 'CELL') do cell := GetContainer(cell);
  if Assigned(cell) then begin
    cn := EditorID(cell);
    loc := LinksTo(ElementByPath(cell, 'XLCN - Location'));
    if Assigned(loc) then ln := EditorID(loc);
  end;

  px := GetElementEditValues(e, 'DATA\Position\X');
  py := GetElementEditValues(e, 'DATA\Position\Y');

  sl.Add(Format('%s;%s;%s;%s;%s;%s;%s;%s;%s', [
    GetFileName(GetFile(e)), IntToHex(GetLoadOrderFormID(e), 8), EditorID(e), nm, tp, cn, ln, px, py]));
end;

function Finalize: integer;
var fn: string;
begin
  fn := ProgramPath + 'Edit Scripts\MapMarkers.csv';
  sl.SaveToFile(fn);
  AddMessage('Exported ' + IntToStr(sl.Count - 1) + ' map markers to ' + fn);
  sl.Free;
  Result := 0;
end;

end.
