// debug_new.cpp
// compile by using: cl /EHsc /W4 /D_DEBUG /MDd debug_new.cpp
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include "dotslib/DotsGame.hpp"
#include <algorithm>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

#include <stdio.h>

#include <GL/glew.h>            // Initialize with glewInit()
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "icons_font_awesome.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"
#define LAY_IMPLEMENTATION
// stb image can read
// png jpg tga bmp psd gif hdr pic

#include <cmath>
#include <memory>

bool resized = false;
const int first_player_color = IM_COL32(0, 255, 255, 255);
const int second_player_color = IM_COL32(255, 255, 0, 255);
const int valid_color = IM_COL32(0, 255, 0, 255);
const int invalid_color = IM_COL32(255, 0, 0, 255);

const int BOARD_H = 30;
const int BOARD_W = 30;

DotsGame game(BOARD_H, BOARD_W);

void GameWindow(std::string name)
{
    ImGuiWindowFlags window_flags = 0;
    ImGui::Begin(name.c_str(), NULL, window_flags);
    float x = ImGui::GetIO().MousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
    float y = ImGui::GetIO().MousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY();

    if (!ImGui::IsWindowCollapsed())
    {
        ImGui::BeginChild((name + "_child").c_str());
        ImVec2 avail_size = ImGui::GetContentRegionAvail();
        ImVec2 p0 = ImGui::GetCursorScreenPos();
        auto draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRect(p0, ImVec2(p0.x + avail_size.x, p0.y + avail_size.y), true);
        int client_width = avail_size.x - 1;
        int client_height = avail_size.y - 1;
        int NcellsX = game.graph_w-1;
        int NcellsY = game.graph_h-1;
        float StepX = float(client_width) / float(NcellsX);
        float StepY = float(client_height) / float(NcellsY);

        int mouseJ = round((x) / StepX);
        int mouseI = round((y) / StepY);

        bool mouseValid = false;
        if (x >= 0 && x < client_width && y >= 0 && y < client_height)
        {
            mouseValid = true;
        }

        if (mouseValid)
        {            
            if (ImGui::GetIO().MouseClicked[0] && !ImGui::GetIO().KeyShift)
            {                
                game.doMove(mouseI, mouseJ);
            }

            if (ImGui::GetIO().MouseClicked[0] && ImGui::GetIO().KeyShift)
            {
                game.graph[mouseI][mouseJ] = 0;
                game.updateBoard();
            }
        }

        ImVec2 pm = ImVec2(float(mouseJ) * StepX, float(mouseI) * StepY) + p0;
        draw_list->AddCircle(pm, 4, IM_COL32(255, 255, 255, 255));

        for (int i = 0; i <= NcellsX; ++i)
        {
            ImVec2 p1, p2;
            p1 = ImVec2(float(i) * StepX, 0) + p0;
            p2 = ImVec2(float(i) * StepX, client_height) + p0;
            draw_list->AddLine(p1, p2, IM_COL32(255, 255, 0, 255));
        }
        for (int i = 0; i <= NcellsY; ++i)
        {
            ImVec2 p1, p2;
            p1 = ImVec2(0, float(i) * StepY) + p0;
            p2 = ImVec2(client_width, float(i) * StepY) + p0;

            draw_list->AddLine(p1, p2, IM_COL32(255, 255, 0, 255));
        }

        for (int i = 0; i < game.graph.size(); ++i)
        {
            for (int j = 0; j < game.graph[i].size(); ++j)
            {
                if (game.graph[i][j] == 1)
                {
                    bool captured = game.graph_captured[i][j]==2;
                    ImVec2 p = ImVec2(float(j) * StepX, float(i) * StepY) + p0;
                    draw_list->AddCircleFilled(p, 4, first_player_color);
                    if (captured)
                    {
                        draw_list->AddCircle(p, 5, second_player_color);
                    }
                }
                else if (game.graph[i][j] == 2)
                {
                    ImVec2 p = ImVec2(float(j) * StepX, float(i) * StepY) + p0;
                    bool captured = game.graph_captured[i][j]==1;
                    
                    draw_list->AddCircleFilled(p, 4, second_player_color);
                    if (captured)
                    {
                        draw_list->AddCircle(p, 5, first_player_color);
                    }
                }

                if (!game.moveIsValid(i, j))
                {
                    ImVec2 p = ImVec2(float(j) * StepX, float(i) * StepY) + p0;
                    draw_list->AddCircle(p, 5, invalid_color);
                }

            }
        }

        if (!game.contours_first.empty())
        {
            for (int i = 0; i < game.contours_first.size(); ++i)
            {
                if (game.contours_first_count[i] == 0)
                {
                    continue;
                }
                for (int j = 0; j < game.contours_first[i].size(); ++j)
                {
                    ImVec2 p1, p2;
                    p1 = ImVec2(game.contours_first[i][j].col * StepX, game.contours_first[i][j].row * StepY) + p0;
                    p2 = ImVec2(game.contours_first[i][(j + 1) % game.contours_first[i].size()].col * StepX, game.contours_first[i][(j + 1) % game.contours_first[i].size()].row * StepY) + p0;
                    draw_list->AddLine(p1, p2, first_player_color, 3);
                }
            }
        }

        if (!game.contours_second.empty())
        {
            for (int i = 0; i < game.contours_second.size(); ++i)
            {
                if (game.contours_second_count[i] == 0)
                {
                    continue;
                }
                for (int j = 0; j < game.contours_second[i].size(); ++j)
                {
                    ImVec2 p1, p2;
                    p1 = ImVec2(game.contours_second[i][j].col * StepX, game.contours_second[i][j].row * StepY) + p0;
                    p2 = ImVec2(game.contours_second[i][(j + 1) % game.contours_second[i].size()].col * StepX, game.contours_second[i][(j + 1) % game.contours_second[i].size()].row * StepY) + p0;
                    draw_list->AddLine(p1, p2, second_player_color, 3);
                }
            }
        }
        // Cursor
        {
            ImVec2 p1, p2;
            p1 = ImVec2(pm.x, p0.y);
            p2 = ImVec2(pm.x, client_height+p0.y);
            draw_list->AddLine(p1, p2, IM_COL32(255, 255, 0, 255), 2);
            p1 = ImVec2(p0.x, pm.y);
            p2 = ImVec2(client_width+p0.x, pm.y);
            draw_list->AddLine(p1, p2, IM_COL32(255, 255, 255, 255), 2);

        }

        draw_list->PopClipRect();
        ImGui::EndChild();
    }
    ImGui::End();
}


void ShowAppDockSpace(bool* p_open);
// Main window
GLFWwindow* window;
bool initialized = false;
int win_width;
int win_height;
int sidePanelWidth;

float toolbarSize = 50;
float statusbarSize = 50;
float menuBarHeight = 0;

void DockSpaceUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    // Save off menu bar height for later.
    //menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

    ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbarSize + menuBarHeight));
    ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbarSize + menuBarHeight + statusbarSize));

    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Master DockSpace", NULL, window_flags);
    ImGuiID dockMain = ImGui::GetID("MyDockspace");
    ImGui::DockSpace(dockMain, ImVec2(0.0f, 0.0f));
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void ToolbarUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize + menuBarHeight));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        ;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
    ImGui::Begin("TOOLBAR", NULL, window_flags);
    ImGui::PopStyleVar();
    menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();
    ImGui::Button(ICON_FA_FILE, ImVec2(0, 37));
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FOLDER_OPEN_O, ImVec2(0, 37));
    ImGui::SameLine();
    ImGui::Button(ICON_FA_FLOPPY_O, ImVec2(0, 37));


    // ----------------------------------------------------
    // Этрисовка меню
    // ----------------------------------------------------
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load", "", false)) { ; }
            if (ImGui::MenuItem("Save", "", false)) { ; }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "", false)) { ; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }


    ImGui::End();
    ImGui::PopStyleColor();
}

void StatusbarUI()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusbarSize));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusbarSize));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        ;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
    ImGui::Begin("STATUSBAR", NULL, window_flags);
    ImGui::PopStyleVar();
    ImGui::Text("Status bar message.");
    ImGui::End();
    ImGui::PopStyleColor();
}
// -----------------------------
// Organize our dockspace
// -----------------------------
void ProgramUI()
{
    DockSpaceUI();
    ToolbarUI();
    StatusbarUI();
}

// -----------------------------
// 
// -----------------------------
std::chrono::steady_clock::time_point prev_time = std::chrono::steady_clock::now();
void render()
{
    ImGui::GetIO().WantCaptureMouse = true;
    glfwPollEvents();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Render our dock (menu, toolbar, status bar).
    ProgramUI();
    // Rendering user content
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // std::cout << "Elapseed = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Elapseed (frame)= " << std::chrono::duration_cast<std::chrono::microseconds>(end - prev_time).count() << "[µs]" << std::endl;
    prev_time = end;

    ImGui::Begin(u8"Control", nullptr);
    if (ImGui::Button(u8"Сброс", ImVec2(-1, 0)))
    {
        game.renderToFile("screen.bmp");
        game.reset(BOARD_H, BOARD_W);
        // ------------------------------
    }
    if (ImGui::Button(u8"Поменяться", ImVec2(-1, 0)))
    {
        game.flipPlayers();
    }

    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, first_player_color);
    char buf[255];   
    sprintf(buf, u8"Земля:%.1f",game.area_of_first);
    ImGui::Text(buf);
    sprintf(buf, u8"Пленные:%.1f", game.captured_by_first);    
    ImGui::Text(buf);
    sprintf(buf, u8"Счет:%.1f", game.getScore(1));
    ImGui::Text(buf);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, second_player_color);
    sprintf(buf, u8"Земля:%.1f", game.area_of_second);
    ImGui::Text(buf);
    sprintf(buf, u8"Пленные:%.1f", game.captured_by_second);
    ImGui::Text(buf);    
    sprintf(buf, u8"Счет:%.1f", game.getScore(2));
    ImGui::Text(buf);
    ImGui::PopStyleColor();

    ImGui::End();
   // static int owner=1;
   // owner = !owner;
   // game.doRandomMove(owner + 1);

    if (game.IsGameOver())
    {
        game.reset(BOARD_H, BOARD_W);
    }
    GameWindow("main window");

    // ImGui rendering
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void resize_window_callback(GLFWwindow* glfw_window, int x, int y)
{
    if (x == 0 || y == 0)
    {
        return;
    }
    if (initialized)
    {
        render();
    }
    resized = true;
}
// -----------------------------
// Initializing openGL things
// -----------------------------
void InitGraphics()
{
    // Setup window
    //glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return;
    }

    // GL 3.0 + GLSL 130
    char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // Create window with graphics context
    window = glfwCreateWindow(win_width, win_height, "CGAL Framework", NULL, NULL);
    if (window == NULL)
    {
        return;
    }


    //windows[1] = glfwCreateWindow(400, 400, "Second", NULL, windows[0]);
    // https://www.khronos.org/opengl/wiki/OpenGL_and_multithreading

   // HGLRC glrc1 = wglCreateContext(window);
   // wglShareLists((HGLRC)window,NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Load Fonts        
    io.Fonts->AddFontFromFileTTF("fonts/FiraCode/ttf/FiraCode-Regular.ttf", 30, NULL, io.Fonts->GetGlyphRangesCyrillic());
    // merge in icons from Font Awesome
    static  ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("fonts/fontawesome-webfont.ttf", 30.0f, &icons_config, icons_ranges);

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }

    glfwSetWindowSizeCallback(window, resize_window_callback);
    initialized = true;
}


// -----------------------------
// Free graphic resources
// -----------------------------
void TerminateGraphics(void)
{
    initialized = false;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}


void main(void)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    //LocaleName = "ru_RU.utf8";
    //setlocale(LC_ALL, LocaleName.c_str());
    // Size of window
    win_width = 1024;
    win_height = 768;
    // Width of side panel
    sidePanelWidth = 300;
    static bool open = true;
    // initialize openGL stuff.
    InitGraphics();
    // -------------
    // Main loop
    // -------------    
    while (!glfwWindowShouldClose(window))
    {
        glfwMakeContextCurrent(window);
        if (initialized)
        {
            if (resized)
            {
                resized = false;
                // do something on main window resize finish
            }
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    TerminateGraphics();
}