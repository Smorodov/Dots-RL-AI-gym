// https://forum.playdots.ru/

#pragma once
#include "dotslib/contours.hpp"
#include "blend2d.h"

#include <random>

#ifdef PYTHON_BIND
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;
#endif

// https://github.com/openai/gym/blob/master/docs/creating_environments.md
// https://stable-baselines3.readthedocs.io/en/master/guide/custom_env.html
class DotsGame
{
public:
    int graph_w;
    int graph_h;
    int currentPlayer;
    float captured_by_second;
    float area_of_second;
    float captured_by_first;
    float area_of_first;
    float free_area;
    std::vector<std::vector<int>> graph;    
    std::vector<std::vector<int>> graph_captured;
    std::vector<std::vector<int>> graph_occupied;
    std::vector<GraphNode> free_positions;
    std::vector<std::vector<GraphNode>> contours_first;
    std::vector<int> contours_first_count;   
    std::vector<std::vector<GraphNode>> contours_second;
    std::vector<int> contours_second_count;
    
    private:
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937* gen; //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<>* distrib_w;
        std::uniform_int_distribution<>* distrib_h;

    std::vector<std::vector<int>> graph_tmp;
    std::vector<std::vector<GraphNode>> contours;
    std::vector<HierarchyNode> hierarchy;
    
    public:

    inline void flipPlayers()
    {
        std::swap(captured_by_second, captured_by_first);
        std::swap(area_of_second, area_of_first);
        flipGraph(graph);
        flipGraph(graph_captured);
        std::swap(contours_first, contours_second);
        std::swap(contours_first_count, contours_second_count);
    }

    inline void renderToFile(std::string filename)
    {
        float cell_size = 10;
        int client_width = graph_w * cell_size;
        int client_height = graph_h * cell_size;

        BLImage* canvas;
        BLContext ctx;
        BLGradient background_gradient;
        canvas = new BLImage(client_width,client_height,BLFormat::BL_FORMAT_PRGB32);
        ctx = BLContext(*canvas);
        // There are many overloads available in C++ API.
        background_gradient.create(
            BLLinearGradientValues(0, 0, canvas->width(), canvas->height()),
            BL_EXTEND_MODE_PAD);
        background_gradient.addStop(0.0, BLRgba32(0xff000000));// abgr
        background_gradient.addStop(1.0, BLRgba32(0xff000000));// abgr
        /*
        BLFontFace face;
        BLFont font;
        BLResult err = face.createFromFile("fonts/FiraCode/ttf/FiraCode-Regular.ttf");
        // We must handle a possible error returned by the loader.
        if (err)
        {
            printf("Failed to load a font-face (err=%u)\n", err);
        }
        font.createFromFace(face, 24.0f);
        */
        ctx.begin(*canvas);
        ctx.setCompOp(BL_COMP_OP_SRC_COPY);
        ctx.setFillStyle(background_gradient);
        ctx.fillAll();
        
        int NcellsX = graph_w;
        int NcellsY = graph_h;
        float StepX = float(client_width) / float(NcellsX);
        float StepY = float(client_height) / float(NcellsY);
        ctx.translate(StepX/2,StepY/2);
        
        ctx.setStrokeStyle(BLRgba32(0xFFff6666));
        ctx.setStrokeWidth(1);
        for (int i = 0; i <= NcellsX; ++i)
        {
            float p1x, p1y, p2x, p2y;
            p1x = float(i) * StepX;
            p1y = 0;

            p2x = float(i) * StepX;
            p2y = client_height;
            ctx.strokeLine(p1x,p1y,p2x,p2y);            
        }
        for (int i = 0; i <= NcellsY; ++i)
        {
            float p1x, p1y, p2x, p2y;            
            p1x = 0;
            p1y = float(i) * StepY;
            
            p2x = client_width;
            p2y = float(i) * StepY;
            ctx.strokeLine(p1x, p1y, p2x, p2y);
        }

        for (int i = 0; i < graph.size(); ++i)
        {
            for (int j = 0; j < graph[i].size(); ++j)
            {
                if (graph[i][j] == 1)
                {
                    bool captured = graph_captured[i][j] == 2;
                    float px = float(j) * StepX;
                    float py = float(i)* StepY;

                    ctx.setStrokeStyle(BLRgba32(0xFF00ffff));
                    ctx.setFillStyle(BLRgba32(0xFF00ffff));
                    ctx.fillCircle(px, py, 3);
                                        
                    if (captured)
                    {
                        ctx.setStrokeStyle(BLRgba32(0xFFffff00));
                        ctx.strokeCircle(px, py, 5);                        
                    }
                }
                else if (graph[i][j] == 2)
                {
                    float px = float(j) * StepX;
                    float py = float(i) * StepY;

                    bool captured = graph_captured[i][j] == 1;

                    ctx.setStrokeStyle(BLRgba32(0xFFffff00));
                    ctx.setFillStyle(BLRgba32(0xFFffff00));
                    ctx.fillCircle(px, py, 3);

                    if (captured)
                    {
                        ctx.setStrokeStyle(BLRgba32(0xFF00ffff));
                        ctx.strokeCircle(px, py, 5);
                    }
                }

                if (moveIsValid(i, j))
                {
                    float px = float(j) * StepX;
                    float py = float(i) * StepY;
                    ctx.setStrokeStyle(BLRgba32(0xFF00ff00));
                    ctx.setFillStyle(BLRgba32(0xFF00ff00));
                    ctx.strokeCircle(px, py, 4);                    
                }

            }
        }


        if (!contours_first.empty())
        {
            for (int i = 0; i < contours_first.size(); ++i)
            {
                if (contours_first_count[i] == 0)
                {
                    continue;
                }
                for (int j = 0; j < contours_first[i].size(); ++j)
                {                    
                    float p1x = contours_first[i][j].col * StepX;
                    float p1y = contours_first[i][j].row * StepY;
                    float p2x = contours_first[i][(j + 1) % contours_first[i].size()].col * StepX;
                    float p2y = contours_first[i][(j + 1) % contours_first[i].size()].row* StepY;
                    ctx.setStrokeStyle(BLRgba32(0xFF00ffff));
                    ctx.setFillStyle(BLRgba32(0xFF00ffff));
                    ctx.strokeLine(p1x, p1y, p2x, p2y);                    
                }
            }
        }

        if (!contours_second.empty())
        {
            for (int i = 0; i < contours_second.size(); ++i)
            {
                if (contours_second_count[i] == 0)
                {
                    continue;
                }
                for (int j = 0; j < contours_second[i].size(); ++j)
                {
                    float p1x = contours_second[i][j].col * StepX;
                    float p1y = contours_second[i][j].row * StepY;
                    float p2x = contours_second[i][(j + 1) % contours_second[i].size()].col * StepX;
                    float p2y = contours_second[i][(j + 1) % contours_second[i].size()].row * StepY;
                    ctx.setStrokeStyle(BLRgba32(0xFFffff00));
                    ctx.setFillStyle(BLRgba32(0xFFffff00));
                    ctx.strokeLine(p1x, p1y, p2x, p2y);
                }
            }
        }


        ctx.end();
        // Let's use some built-in codecs provided by Blend2D.
        BLImageCodec codec;
        codec.findByName("BMP");
        canvas->writeToFile(filename.c_str(), codec);
        delete canvas;
    }

    inline void InitGraph(std::vector<std::vector<int>>& graph, int graph_w, int graph_h)
    {
        graph = std::vector< std::vector<int> >(graph_h);
        for (auto& l : graph)
        {
            l.resize(graph_w, 0);
        }
    }

    // flip players in graph 1 <-> 2
    inline void flipGraph(std::vector<std::vector<int>>& graph)
    {
        for (int i = 0; i < graph.size(); ++i)
        {
            for (int j = 0; j < graph[i].size(); ++j)
            {
                if (graph[i][j] == 1)
                {
                    graph[i][j] = 2;
                }
                else if (graph[i][j] == 2)
                {
                    graph[i][j] = 1;
                }
            }
        }
    }

    inline void getGraph(std::vector<std::vector<int>>& graph,
        std::vector<std::vector<int>>& graph_captured ,
        int player, std::vector<std::vector<int>>& result)
    {
        for (int i = 0; i < graph.size(); ++i)
        {
            for (int j = 0; j < graph[i].size(); ++j)
            {
                if (graph[i][j] == player && (graph_captured[i][j] == player || graph_captured[i][j] == 0))
                {
                    result[i][j] = 1;
                }
                else
                {
                    result[i][j] = 0;
                }
            }
        }
    }

    inline void resetGraph(std::vector<std::vector<int>>& graph)
    {
        for (int i = 0; i < graph.size(); ++i)
        {
            for (int j = 0; j < graph[i].size(); ++j)
            {
                graph[i][j] = 0;
            }
        }
    }

    inline void cleanGraph(std::vector<std::vector<int>>& graph)
    {
        for (int i = 0; i < graph.size(); ++i)
        {
            graph[i].clear();
        }
        graph.clear();
    }

    void cleanContours(std::vector<std::vector<GraphNode>>& contours)
    {
        for (int i = 0; i < contours.size(); ++i)
        {
            contours[i].clear();
        }
        contours.clear();
    }

    DotsGame(int rows,int cols)
    {
        captured_by_second=0;
        area_of_second=0;
        captured_by_first=0;
        area_of_first=0;
        free_area = cols * rows;
        currentPlayer = 1;
        graph_w=cols;
        graph_h=rows;
        gen = new std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()
        distrib_w = new std::uniform_int_distribution<>(0, graph_w);
        distrib_h = new std::uniform_int_distribution<>(0, graph_h);

        InitGraph(graph, graph_w, graph_h);
        InitGraph(graph_tmp, graph_w, graph_h);
        InitGraph(graph_captured, graph_w, graph_h);
        InitGraph(graph_occupied, graph_w, graph_h);
        
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                free_positions.push_back(GraphNode(i, j));
            }
        }
    }

    inline void clean()
    {
        free_positions.clear();
        captured_by_second=0;
        area_of_second=0;
        captured_by_first=0;
        area_of_first=0;
        delete gen; //Standard mersenne_twister_engine seeded with rd()
        delete distrib_w;
        delete distrib_h;

        cleanGraph(graph);
        cleanGraph(graph_tmp);
        cleanGraph(graph_captured);
        cleanGraph(graph_occupied);        
        cleanContours(contours_first);
        contours_first_count.clear();
        cleanContours(contours_second);
        contours_second_count.clear();                
        cleanContours(contours);
        hierarchy.clear();
        graph_w = 0;
        graph_h = 0;
        free_area = 0;
    }

    inline void reset(int rows, int cols)
    {
        clean();
        graph_w = cols;
        graph_h = rows;
        
        gen = new std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()
        distrib_w = new std::uniform_int_distribution<>(0, graph_w);
        distrib_h = new std::uniform_int_distribution<>(0, graph_h);


        free_area = cols * rows;
        
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                free_positions.push_back(GraphNode(i, j));
            }
        }
        currentPlayer = 1;
        InitGraph(graph, graph_w, graph_h);
        InitGraph(graph_tmp, graph_w, graph_h);
        InitGraph(graph_captured, graph_w, graph_h);
        InitGraph(graph_occupied, graph_w, graph_h);
    }

    ~DotsGame()
    {
        clean();
    }


    void updateBoard()
    {
        contours_first_count.clear();
        contours_second_count.clear();                       
        // ------------------------------
        getGraph(graph,graph_captured, 1, graph_tmp);
        contours.clear();
        hierarchy.clear();
        contours_first.clear();       
        detectContours(graph_tmp, contours, hierarchy);
        if (!hierarchy.empty())
        {
            for (int i = 0; i < hierarchy.size(); ++i)
            {
                if (hierarchy[i].parent == 1)
                {
                    std::vector<GraphNode> contour;
                    --i;
                    for (int j = 0; j < contours[i].size(); ++j)
                    {
                        contour.push_back(GraphNode(contours[i][j].row, contours[i][j].col));
                    }
                    ++i;
                    contours_first.push_back(contour);

                }
            }
        }
        // ------------------------------
        getGraph(graph, graph_captured, 2, graph_tmp);
        contours.clear();
        hierarchy.clear();
        contours_second.clear();
        
        detectContours(graph_tmp, contours, hierarchy);
        
        if (!hierarchy.empty())
        {
            for (int i = 0; i < hierarchy.size(); ++i)
            {
                if (hierarchy[i].parent == 1)
                {
                    std::vector<GraphNode> contour;
                    --i;
                    for (int j = 0; j < contours[i].size(); ++j)
                    {
                        contour.push_back(GraphNode(contours[i][j].row, contours[i][j].col));
                    }
                    ++i;
                    contours_second.push_back(contour);
                }
            }
        }

        contours_second_count.resize(contours_second.size(), 0);
        int contInd = 0;
        for (auto contour : contours_second)
        {
            int counter = 0;
            if (contour.size() <= 3)
            {
                contours_second_count[contInd] = 0;
                ++contInd;
                continue;
            }
            
            for (int i = 0; i < graph.size(); ++i)
            {
                for (int j = 0; j < graph[i].size(); ++j)
                {

                    if (graph[i][j] == 1)
                    {
                        bool captured = false;
                        captured = contains(GraphNode(i, j), contour);                        
                        if (captured)
                        {
                            counter++;
                            contours_second_count[contInd]=counter;
                            graph_captured[i][j] = 2;                            
                        }
                        else
                        {
                            if (graph_captured[i][j] == 2)
                            {
                               // graph_captured[i][j] = 0;
                            }
                        }
                    }
                }
            }
            ++contInd;
        }

        contours_first_count.resize(contours_first.size(), 0);
        contInd = 0;
        for (auto contour : contours_first)
        {
            int counter = 0;

            if (contour.size() <= 3)
            {                   
                contours_first_count[contInd] = 0;
                ++contInd;
                continue;
            }

            for (int i = 0; i < graph.size(); ++i)
            {
                for (int j = 0; j < graph[i].size(); ++j)
                {                    
                    if (graph[i][j] == 2)
                    {
                        bool captured = false;
                        captured = contains(GraphNode(i, j), contour);
                        if (captured)
                        {
                            counter++;
                            contours_first_count[contInd]=counter;
                            graph_captured[i][j] = 1;
                        }
                        else
                        {
                            if (graph_captured[i][j] == 1)
                            {
                              //  graph_captured[i][j] = 0;
                            }
                        }
                    }
                }
            }
            ++contInd;
        }

        
        contInd = 0;
        captured_by_first = 0;
        area_of_first = 0;
        for (auto contour : contours_first)
        {            
            if (contours_first_count[contInd] > 0)
            {
                captured_by_first += contours_first_count[contInd];
                area_of_first     += doubleArea(contour);
                for (int i = 0; i < graph.size(); ++i)
                {
                    for (int j = 0; j < graph[i].size(); ++j)
                    {
                        bool captured = contains(GraphNode(i, j), contour);
                        graph_occupied[i][j] = graph_occupied[i][j] || captured;
                    }
                }
            }
            ++contInd;
        }

        contInd = 0;
        captured_by_second = 0;
        area_of_second = 0;
        for (auto contour : contours_second)
        {
            if (contours_second_count[contInd] > 0)
            {
                captured_by_second += contours_second_count[contInd];
                area_of_second     += doubleArea(contour);
                for (int i = 0; i < graph.size(); ++i)
                {
                    for (int j = 0; j < graph[i].size(); ++j)
                    {
                        bool captured = contains(GraphNode(i, j), contour);
                        graph_occupied[i][j] = graph_occupied[i][j] || captured;
                    }
                }
            }
            ++contInd;
        }

        // -------------------------
        // fill occupancy graph        
        // -------------------------
        free_area = 0;
        free_positions.clear();
        for (int i = 0; i < graph.size(); ++i)
        {
            for (int j = 0; j < graph[i].size(); ++j)
            {
                graph_occupied[i][j] = graph_occupied[i][j] || graph[i][j] > 0;                
                if (graph_occupied[i][j] == 0)
                {
                    free_area++;
                    free_positions.push_back(GraphNode(i, j));
                }
            }
        }

    }

    bool moveIsValid(int i, int j)
    {
        if (i < 0 || i >= graph_h || j < 0 || j >= graph_w)
        {
            return false;
        }

        if (graph_occupied[i][j] != 0)
        {
            return false;
        }
        return true;
    }

    float getScore(int player)
    {
        float score = 0;
        if (player == 1)
        {
            score = captured_by_first + area_of_first * 0.25;
        }
        else if(player==2)
        {
            score=captured_by_second+area_of_second*0.25;
        }
        return score;
    }

    // add point of player
    void addPoint(int row,int col, int player)
    {
        if (graph[row][col] == 0)
        {
            graph[row][col] = player;
        }
        updateBoard();
    }

    // do move with player shitched by game
    bool doMove(int row,int col)
    {   
        bool result = false;
        if (moveIsValid(row, col))
        {
            addPoint(row, col, currentPlayer);
            currentPlayer++;
            if (currentPlayer > 2)
            {
                currentPlayer = 1;
            }
            result = true;
        }
        return result;
    }

    // do move with defined player
    bool doMovePlayer(int row, int col, int player)
    {
        bool result = false;
        if (moveIsValid(row, col))
        {
            addPoint(row, col, player);            
            result = true;
        }
        return result;
    }
    bool doRandomMove(int player)
    {   
        if (free_positions.size() == 0)
        {
            return false;
        }

        int ind= rand() % free_positions.size();
        int row = free_positions[ind].row;
        int col = free_positions[ind].col;
        
        if (moveIsValid(row, col))
        {
            addPoint(row, col, player);
            return true;
        }

        return false;
    }

    bool IsGameOver()
    {        
        return (free_area==0);
    }

#ifdef PYTHON_BIND
    py::array_t<unsigned char> getGameState()
    {        
        py::array_t<unsigned char, py::array::c_style> result({3, graph_h, graph_w });
        auto rm = result.mutable_unchecked<3>();

            for (size_t i = 0; i < rm.shape(1); i++) // col
            {
                for (size_t j = 0; j < rm.shape(2); j++) // row
                {
                    rm(0, i, j) =  graph[i][j];
                    rm(1, i, j) =  graph_occupied[i][j];
                    rm(2, i, j) =  graph_captured[i][j];
                }
            }
        
        return result;
    }

    void setGameState(py::array_t<unsigned char>& state)
    {        
        auto im = state.mutable_unchecked<3>();                
        for (size_t i = 0; i < im.shape(1); i++)
        {
            for (size_t j = 0; j < im.shape(2); j++)
            {
                graph[i][j]= im(0, i, j);
                graph_occupied[i][j]= im(1, i, j);
                graph_captured[i][j]= im(2, i, j);
            }
        }
    }

#endif

};