// MarkerSwap F4SE plugin (CommonLibF4 + Address Library).
//  - Exposes f4se.plugins.MarkerSwap.GetIconPath(markerType, location) and .Reload() to ActionScript.
//  - When PipboyMenu opens, loads Data/Interface/MarkerSwap.swf into root.Menu_mc (same idea as PipboyTabs.dll).
// Written against CommonLibF4 headers but NOT run in-game. Fix against build errors / the F4SE log.
#include "F4SE/F4SE.h"
#include "RE/Fallout.h"

#include "IconResolver.h"

using namespace std::literals;
namespace GFx = Scaleform::GFx;

namespace
{
	std::unique_ptr<IconResolver> g_resolver;
	const std::filesystem::path   kRoot = "Data/Interface/MapMarkers";

	class GetIconPath : public GFx::FunctionHandler
	{
	public:
		void Call(const Params& a_params) override
		{
			// args: [0]=marker type ("SettlementMarker"), [1]=optional location editorID / 0xFormID
			*a_params.retVal = nullptr;
			if (a_params.argCount < 1 || !g_resolver || !a_params.args[0].IsString()) return;
			const std::string type = a_params.args[0].GetString();
			const std::string loc = (a_params.argCount > 1 && a_params.args[1].IsString()) ? a_params.args[1].GetString() : "";
			if (auto p = g_resolver->Resolve(type, loc)) {
				const auto rel = std::filesystem::relative(*p, "Data").generic_string();  // relative to Data/
				a_params.movie->asMovieRoot->CreateString(a_params.retVal, rel.c_str());
			}
		}
	};

	class Reload : public GFx::FunctionHandler
	{
	public:
		void Call(const Params&) override
		{
			if (g_resolver) g_resolver->Reload();
		}
	};

	bool F4SEAPI RegisterScaleform(GFx::Movie* a_view, GFx::Value* a_f4se)
	{
		static GetIconPath getIconPath;
		static Reload      reload;
		GFx::Value         obj, fn;
		a_view->CreateObject(&obj);
		a_view->CreateFunction(&fn, &getIconPath);
		obj.SetMember("GetIconPath", fn);
		a_view->CreateFunction(&fn, &reload);
		obj.SetMember("Reload", fn);
		a_f4se->SetMember("MarkerSwap", obj);
		return true;
	}

	class MenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_evn, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
		{
			if (a_evn.opening && std::string_view(a_evn.menuName) == "PipboyMenu"sv) {
				if (g_resolver) g_resolver->Reload();  // pick up newly dropped PNGs each time the Pip-Boy opens
				if (const auto ui = RE::UI::GetSingleton()) {
					if (const auto menu = ui->GetMenu("PipboyMenu"); menu && menu->uiMovie) {
						auto&     movie = *menu->uiMovie;
						GFx::Value url{ "Interface/MarkerSwap.swf" };
						GFx::Value loader, req, menuMc;
						movie.CreateObject(&loader, "flash.display.Loader");
						movie.CreateObject(&req, "flash.net.URLRequest", &url, 1);
						loader.Invoke("load", nullptr, &req, 1);
						if (movie.GetVariable(&menuMc, "root.Menu_mc")) menuMc.Invoke("addChild", nullptr, &loader, 1);
					}
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};
	MenuWatcher g_watcher;

	void F4SEAPI OnMessage(F4SE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == static_cast<std::uint32_t>(F4SE::MessagingInterface::kGameDataReady)) {
			g_resolver = std::make_unique<IconResolver>(kRoot);
			if (const auto ui = RE::UI::GetSingleton()) ui->RegisterSink<RE::MenuOpenCloseEvent>(&g_watcher);
			REX::INFO("MarkerSwap: {} type icons, {} location icons", g_resolver->TypeCount(), g_resolver->LocationCount());
		}
	}
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);
	F4SE::GetMessagingInterface()->RegisterListener(OnMessage);
	F4SE::GetScaleformInterface()->Register("MarkerSwap"sv, RegisterScaleform);
	return true;
}
