// Document class of MarkerSwap.swf. Loaded into PipboyMenu.Menu_mc by the plugin.
// For every vanilla map marker clip, hide it and draw the PNG chosen by the plugin at the same place.
// PNG is drawn in a sibling, so the game's ColorTransform on the vanilla clip never touches it (colours kept).
package {
    import flash.display.*;
    import flash.events.*;
    import flash.net.URLRequest;
    import flash.utils.*;
    import flash.geom.ColorTransform;

    public class MarkerSwap extends MovieClip {
        private static const POLL:int = 15;          // frames between scans
        private var plugin:Object;
        private var tick:int = 0;
        private var swaps:Dictionary = new Dictionary(true);   // vanilla clip -> replacement Loader

        public function MarkerSwap() {
            addEventListener(Event.ADDED_TO_STAGE, onStage);
        }
        private function onStage(e:Event):void {
            var r:Object = stage.getChildAt(0);
            plugin = r.f4se ? r.f4se.plugins.MarkerSwap : null;
            addEventListener(Event.ENTER_FRAME, onFrame);
        }
        private function onFrame(e:Event):void {
            if (!plugin) return;
            // keep replacements glued to their vanilla clip every frame
            for (var k:* in swaps) sync(k as DisplayObject, swaps[k] as DisplayObject);
            if (++tick % POLL) return;
            scan(stage.getChildAt(0) as DisplayObjectContainer, 0);
        }
        private function scan(c:DisplayObjectContainer, depth:int):void {
            if (!c || depth > 8) return;
            for (var i:int = 0; i < c.numChildren; i++) {
                var ch:DisplayObject = c.getChildAt(i);
                var cn:String = getQualifiedClassName(ch);
                if (cn.indexOf("Marker") >= 0 && !(ch is Loader) && !(ch in swaps)) {
                    var path:String = plugin.GetIconPath(cn, ch.name);
                    if (path) replace(ch, path);
                } else if (ch is DisplayObjectContainer && !(ch in swaps)) {
                    scan(ch as DisplayObjectContainer, depth + 1);
                }
            }
        }
        private function replace(vanilla:DisplayObject, path:String):void {
            var l:Loader = new Loader();
            l.load(new URLRequest(path));        // PNG relative to Data/ (adjust if the game resolves differently)
            vanilla.parent.addChild(l);
            swaps[vanilla] = l;
            sync(vanilla, l);
        }
        private function sync(v:DisplayObject, l:DisplayObject):void {
            if (!v.parent) { l.visible = false; return; }
            v.alpha = 0;                          // hide vanilla but keep it alive for game logic
            l.x = v.x; l.y = v.y; l.scaleX = v.scaleX; l.scaleY = v.scaleY;
            l.visible = v.visible; l.alpha = 1;
            // identity colour transform: the PNG's own colours
            l.transform.colorTransform = new ColorTransform();
        }
    }
}
