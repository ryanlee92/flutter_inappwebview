#ifndef FLUTTER_INAPPWEBVIEW_PLUGIN_IN_APP_BROWSER_H_
#define FLUTTER_INAPPWEBVIEW_PLUGIN_IN_APP_BROWSER_H_

#include <optional>
#include <string>
#include <Windows.h>

#include "../flutter_inappwebview_windows_plugin.h"
#include "../in_app_webview/in_app_webview.h"
#include "../in_app_webview/in_app_webview_settings.h"
#include "../types/url_request.h"
#include "in_app_browser_channel_delegate.h"
#include "in_app_browser_settings.h"

namespace flutter_inappwebview_plugin
{
  struct InAppBrowserCreationParams
  {
    const std::string id;
    const std::optional<std::shared_ptr<URLRequest>> urlRequest;
    const std::optional<std::string> assetFilePath;
    const std::optional<std::string> data;
    const std::shared_ptr<InAppBrowserSettings> initialSettings;
    const std::shared_ptr<InAppWebViewSettings> initialWebViewSettings;
    const std::optional<std::vector<std::shared_ptr<UserScript>>> initialUserScripts;
    const std::optional<std::string> webViewEnvironmentId;
  };

  class InAppBrowser {
  public:
    static inline const std::string METHOD_CHANNEL_NAME_PREFIX = "com.pichillilorenzo/flutter_inappbrowser_";
    static inline const wchar_t* CLASS_NAME = L"InAppBrowser";

    static LRESULT CALLBACK WndProc(HWND window,
      UINT message,
      WPARAM wparam,
      LPARAM lparam) noexcept;

    const FlutterInappwebviewWindowsPlugin* plugin;
    const std::string id;
    std::unique_ptr<InAppWebView> webView;
    std::unique_ptr<InAppBrowserChannelDelegate> channelDelegate;
    std::shared_ptr<InAppBrowserSettings> settings;

    InAppBrowser(const FlutterInappwebviewWindowsPlugin* plugin, const InAppBrowserCreationParams& params);
    ~InAppBrowser();

    void close() const;
    void show() const;
    void hide() const;
    bool isHidden() const;
    void setSettings(const std::shared_ptr<InAppBrowserSettings> newSettings, const flutter::EncodableMap& newSettingsMap);
    flutter::EncodableValue getSettings() const;

    void didChangeTitle(const std::optional<std::string>& title) const;
    HWND getHWND() const
    {
      return m_hWnd;
    }
  private:
    const HINSTANCE m_hInstance;
    HWND m_hWnd;
    bool destroyed_ = false;
    static InAppBrowser* GetThisFromHandle(HWND window) noexcept;
    LRESULT MessageHandler(HWND window,
      UINT message,
      WPARAM wparam,
      LPARAM lparam) noexcept;

     // ⬇️ 추가: WebView2 이벤트 토큰
    EventRegistrationToken nav_starting_token_{};
    EventRegistrationToken new_window_token_{};

    // ⬇️ 추가: 헬퍼/가드 설치
    static std::wstring ToLower(std::wstring s) {
      std::transform(s.begin(), s.end(), s.begin(), ::towlower);
      return s;
    }
    static std::wstring ExtractScheme(const std::wstring& uri) {
      auto pos = uri.find(L":");
      if (pos == std::wstring::npos) return L""; // 상대경로/프래그먼트 등
      return ToLower(uri.substr(0, pos));
    }
    bool IsCustomScheme(const std::wstring& uri) const {
      auto scheme = ExtractScheme(uri);
      if (scheme.empty()) return false; // 상대경로는 허용

      // 화이트리스트 직접 체크
      return !(scheme == L"http" ||
              scheme == L"https" ||
              scheme == L"file" ||
              scheme == L"about" ||
              scheme == L"data" ||
              scheme == L"blob");
    }

    void SetupNavigationGuards();
  };
}
#endif //FLUTTER_INAPPWEBVIEW_PLUGIN_IN_APP_BROWSER_H_
