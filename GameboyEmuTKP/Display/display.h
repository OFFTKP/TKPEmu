#ifndef TKP_GB_DISPLAY_H
#define TKP_GB_DISPLAY_H
#include <glad/glad.h>
#include <SDL.h>
#include <string>
#include <type_traits>
#include <mutex>
#include <map>
#include "../Tools/prettyprinter.h"
#include "../ImGui/imgui_impl_sdl.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/imgui_internal.h"
// Helper Macros - IM_FMTARGS, IM_FMTLIST: Apply printf-style warnings to our formatting functions.
#if !defined(IMGUI_USE_STB_SPRINTF) && defined(__MINGW32__) && !defined(__clang__)
#define IM_FMTARGS(FMT)             __attribute__((format(gnu_printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(gnu_printf, FMT, 0)))
#elif !defined(IMGUI_USE_STB_SPRINTF) && (defined(__clang__) || defined(__GNUC__))
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif

namespace TKP::Graphics {
    constexpr auto UserSettingsFile = "tkpuser.ini";
    struct WindowSettings {
        int window_width = 640;
        int window_height = 480;
        int minimum_width = 640;
        int minimum_height = 480;
        int maximum_width = 1920;
        int maximum_height = 1080;
    };
    enum class FileAccess { Read, Write };
	class Display {
    private:
        using PrettyPrinter = TKP::Tools::PrettyPrinter;
        using SDL_GLContextType = std::remove_pointer_t<SDL_GLContext>;
        using PPMessageType = TKP::Tools::PrettyPrinterMessageType;
    private:
    private:
        // RAII class for the initialization functions
        class DisplayInitializer {
        public:
            DisplayInitializer(PrettyPrinter&);
            ~DisplayInitializer();
        };
    public:
        Display();
        Display(const Display&) = delete;
        Display& operator=(const Display&) = delete;
        void EnterMainLoop();
    private:
        // This member being first means that it gets constructed first and gets destructed last
        // which is what we want to happen with the SDL_Init and the destroy functions
        DisplayInitializer display_initializer_;

        // PrettyPrinter class is used to add strings to a buffer that
        // the user can later get to print however they want.
        PrettyPrinter pretty_printer_;
        WindowSettings window_settings_;

        const std::string glsl_version = "#version 430";

        std::unique_ptr<SDL_GLContextType, decltype(&SDL_GL_DeleteContext)> gl_context_ptr_;
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_ptr_;

        // State bools
        bool rom_loaded_ = false;

        // These bools determine whether certain windows are open
        // We aren't allowed to use static member variables so we use static functions that
        // return static members
        bool menu_bar_open_ = true;
        bool window_tracelogger_open_ = false;
        bool window_fpscounter_open_ = false;

        // Window drawing functions for ImGui
        void draw_settings(bool* draw);
        void draw_trace_logger(bool* draw);
        void draw_fps_counter(bool* draw);
        void draw_menu_bar(bool* draw);
        void draw_menu_bar_file();
        void draw_menu_bar_file_recent();
        void draw_menu_bar_tools();
        void draw_menu_bar_view();

        void load_user_settings();
        void save_user_settings();
        // This function chooses fread or fwrite on compile time.
        // That way we can just change set_user_settings to add more settings,
        // instead of changing both save/load_user_settings.
        template<FileAccess acc>
        void access_file(void* item, size_t size, size_t count, FILE* stream);
        template<FileAccess acc>
        void set_user_settings(FILE* file);

        // Mutex for writing and reading to gameboy screen buffer, since different
        // threads are going to be accessing it
        std::mutex graphics_mutex_;
        void main_loop();
        
	};
    template<>
    inline void Display::access_file<FileAccess::Read>(void* item, size_t size, size_t count, FILE* stream) {
        fread(item, size, count, stream);
    }
    template<>
    inline void Display::access_file<FileAccess::Write>(void* item, size_t size, size_t count, FILE* stream) {
        fwrite(item, size, count, stream);
    }
    template<FileAccess acc>
    inline void Display::set_user_settings(FILE* file) {
        access_file<acc>(&window_fpscounter_open_, sizeof(bool), size_t(1), file);
    }
}
#endif