#include "Game.hpp"
#include "raylib.h" 

Game::Game( lua_State* L)
{
    this->gameState = GAME_STATE::MENU;

    this->L = L;
    this->scene = new Scene();
    this->scene->lua_openscene(L, this->scene);
    this->map = new mapLoader(this->scene);
    this->editor = new Editor(this->scene);
    this->mapSave = false;

    luaL_dofile(L, "../scripts/vector.lua");
	lua_setglobal(L, "vector");
}

Game::~Game()
{
    //TODO: Check here why I get segmentation fault
    //delete this->scene;
    //delete this->map;
    //delete this->editor;
    std::cout<<"Game deconstructor\n";
}

void Game::run()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    this->scene->init();

    luaL_dofile(this->L, "../scripts/test.lua");

    this->buttons.push_back(Button("Start", "button1.png", {20.0f, 20.0f}, 100, 20, WHITE));

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        checkMenuSwitch();

        switch (this->gameState)
        {
        case GAME_STATE::MENU:
            drawMenu();
            break;
        case GAME_STATE::GAME:
            drawGame();
            break;
        case GAME_STATE::EDITOR:
            drawEditor();
            break;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}

void Game::setSystems()
{
    this->scene->AddSystem("BehaviourSystem",new BehaviourSystem(L));
}

void Game::checkMenuSwitch()
{
    //To menu
    if(IsKeyDown(KEY_M))
    {
        if(this->gameState == GAME_STATE::GAME)
        {
            this->map->unLoad();
        }
        if(this->gameState != GAME_STATE::MENU)
        {
            this->gameState = GAME_STATE::MENU;
            this->startMenu();
        }
    }
    //To game
    else if(IsKeyDown(KEY_G))
    {
        if(this->gameState != GAME_STATE::GAME)
        {
            this->gameState = GAME_STATE::GAME;
            this->startGame();
        }
    }
    //To editor
    else if(IsKeyDown(KEY_E))
    {
        if(this->gameState == GAME_STATE::GAME)
        {
            this->map->unLoad();
        }
        if(this->gameState != GAME_STATE::EDITOR)
        {
            this->gameState = GAME_STATE::EDITOR;
            this->startEditor();
        }
    }
}

void Game::startMenu()
{
    this->scene->resetBehaviours(L);
    this->scene->RemoveSystem("BehaviourSystem");
    //this->scene->setCameraPosition((Vector3){0.0f,0.0f, 10.0f});
}

void Game::drawMenu()
{
    //Update
    this->scene->UpdateSystems(GetFrameTime());
    // Draw
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Current: Menu\nMenu: M\nGame: G\nEditor: E", 190, 200, 20, LIGHTGRAY);
        this->scene->draw();

        for(auto& b: this->buttons)
        {
            b.draw();
            if(b.canClick()){
                std::cout<<"HOVER\n";
            }
            if(b.clicked()){
               this->gameState = GAME_STATE::GAME;
               this->startGame();
            }
        }

    EndDrawing();   
}

void Game::startGame()
{
    //luaL_dofile(this->L, "../scripts/test.lua");
    this->scene->AddSystem("BehaviourSystem",new BehaviourSystem(L));
    this->map->load();
    //this->scene->setCameraPosition((Vector3){0.0f,0.0f, 10.0f});
}

void Game::drawGame()
{
    //Update
    this->scene->UpdateSystems(GetFrameTime());
   
    // Draw
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Current: Game\nMenu: M\nGame: G\nEditor: E", 190, 200, 20, LIGHTGRAY);
        this->scene->draw();
    EndDrawing();   
}

void Game::startEditor()
{
    this->editor->init();
    this->scene->RemoveSystem("BehaviourSystem");
    this->scene->resetBehaviours(L);
    //this->scene->setCameraPosition((Vector3){0.0f,0.0f, 300.0f});

}

void Game::drawEditor()
{
    //Update
    
    if(IsKeyPressed(KEY_ENTER) && !this->mapSave)
    {
        this->mapSave = true;
        this->mapSaveName = "";
    }

    this->scene->UpdateSystems(GetFrameTime());
    if( this->mapSave)
    {
        char c = GetCharPressed();
        if(c != 0)
        {
            std::cout<<"input char: "<<c<<std::endl;
            this->mapSaveName += c;
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            std::cout<<"Pop\n";
            this->mapSaveName.pop_back();
        }
        else if(IsKeyPressed(KEY_ENTER))
        {
            std::cout<<"Save this shit!"<<std::endl;
            if(this->mapSaveName.size() > 0 && this->editor->save(this->mapSaveName))
            {
                this->editor->reset();
                this->mapSaveName = "";
                this->mapSave = false;
            }
            else
            {
                std::cout<<"Name already in use!"<<std::endl;
            }
        }
    }
    else
    {
        this->editor->update();
    }

    // Draw
    BeginDrawing();
        ClearBackground(RAYWHITE);

        this->scene->draw();
        if( this->mapSave)
        {
            std::string disp = "Name: "+this->mapSaveName;
            DrawText(disp.c_str(), 10, 10, 20, LIGHTGRAY);
            DrawText("Current: Editor\nMenu: M\nGame: G\nEditor: E", 190, 200, 20, LIGHTGRAY);
        }
        else
        {
            //BeginMode3D(*this->scene->getCam());
            this->editor->draw();
            //EndMode3D();
            DrawText("1: Ground\n2: Enemy\nEnter: Save\n", 10, 10, 20, LIGHTGRAY);
        }
        
    EndDrawing();   
}