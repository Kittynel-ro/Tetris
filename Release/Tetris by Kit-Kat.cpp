#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

//Declaring board and cell size

int cell_size = 18;

const int board_w = 10;
const int board_h = 22;

//Declaring tetris shapes

int Shapes[7][4][4] = {

    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,

    1,0,0,0,
    1,1,0,0,
    0,1,0,0,
    0,0,0,0,

    0,1,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0,

    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,

    1,0,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0,

    1,0,0,0,
    1,0,0,0,
    1,1,0,0,
    0,0,0,0,

    0,0,0,0,
    0,1,0,0,
    0,1,0,0,
    1,1,0,0,

};

int board[board_h][board_w] = { 0 };
int next_piece[4][4] = { 0 };

int main()
{
   RenderWindow window(sf::VideoMode(board_w * cell_size * 2 + 320, board_h * cell_size * 2), "Tetris by Kit-Kat");

   //window.setFramerateLimit(240);

   //Importing Textures

   Texture cell_skin, bckgrnd_skin, scoreb_skin, over_skin;

   cell_skin.loadFromFile("resources/tiles.png");
   bckgrnd_skin.loadFromFile("resources/bckgrnd.png");
   scoreb_skin.loadFromFile("resources/scoreboard.png");
   over_skin.loadFromFile("resources/gameover.png");

   //Creating the sprites

   Sprite cell(cell_skin), bckgrnd(bckgrnd_skin), scoreb(scoreb_skin),gameover(over_skin);
   
   cell.setTextureRect(IntRect(0, 0, 18, 18));
   cell.scale(Vector2f(2.f, 2.f));
   scoreb.setPosition(Vector2f(board_w * cell_size * 2, 0));
   gameover.setPosition(Vector2f(0, 0));

   //Adding fonts and music

   Text score_t, fps_t;

   Font score_f, fps_f;

   score_f.loadFromFile("resources/aesymatt.ttf");
   fps_f.loadFromFile("resources/aesymatt.ttf");

   Music song;

   song.openFromFile("resources/song.ogg");

   song.setVolume(10);

   song.setLoop(true);

   //Game state, score and shape info variables

   int Score = 0;

   int kind;
   int cx;
   int cy;

   bool game_over = false;
   bool sound_off = false;

   //String streams for score and frames

   ostringstream ss1, ss2;

   score_t.setFont(score_f);
   fps_t.setFont(fps_f);

   auto new_block = [&]()
   {
       srand(time(NULL));
       kind = rand() % 7; cx = board_w / 2 - 1, cy = 0;
   };
   
   new_block();

   auto check_block = [&]()
   {
       for (int y = 0; y < 4; y++)
           for (int x = 0; x < 4; x++)
           {
               if (Shapes[kind][y][x] == 0)
                   continue;
               //Checking collision with walls
               if (cx + x < 0 || cx + x >= board_w || cy + y >= board_h)
                   return false;
               //Checking collision with other shapes
               if (board[cy + y][cx + x])
                   return false;
           }

        return true;
   };

   auto end_game = [&]()
   {
       for (int x = 0; x < 4; x++)
       {
           if (board[0][cx + x] != 0)
           {
               game_over = true;
               window.draw(gameover);
           }
       }
       
   };

   auto restart_game = [&]()
   {
       for (int y = 0; y < board_h; y++)
           for (int x = 0; x < board_w; x++)
               board[y][x] = 0;
      
       Score = 0;
       window.clear();
      
   };

   auto go_down = [&]()
   {
       cy++;

       if (check_block() == false) //IF the piece hits the bottom or an other piece
       {
           cy--;
           for (int y = 0; y < 4; y++)
               for (int x = 0; x < 4; x++)
                   if (Shapes[kind][y][x]) //Add the piece to the board
                   {
                       board[cy + y][cx + x] = kind + 1;
                   }
           new_block();
           return false;
       }
       return true;
   };

   auto rotate = [&]()
   {
       int len = 0; //Declaring piece lenght
       for (int y = 0; y < 4; y++)
           for (int x = 0; x < 4; x++)
                if (Shapes[kind][y][x])
                    len = std::max(std::max(x, y) + 1, len); //Calculating the lenght of the piece

       int d[4][4] = { 0 }; //Aux matrix where we rotate the piece

       for (int y = 0; y < len; y++)
           for (int x = 0; x < len; x++)
               if (Shapes[kind][y][x])
                   d[len - 1 - x][y] = 1; //Copying the piece rotated into the aux matrix

       for (int y = 0; y < 4; y++)
           for (int x = 0; x < 4; x++)
               Shapes[kind][y][x] = d[y][x]; //Copying back the rotated piece
                   
   };

   Clock clock;
   
   float fps;

   Time prev_time = clock.getElapsedTime();
   Time cur_time;

   while (window.isOpen())
   {
       //Calculating the Fps
       static float prev = clock.getElapsedTime().asSeconds();
       if (clock.getElapsedTime().asSeconds() - prev >= 0.5)
       {
           prev = clock.getElapsedTime().asSeconds();
           go_down();
       }

       Event event;
       
       //Window events and keyboard events

       while (window.pollEvent(event))
       {
           if (event.type == sf::Event::Closed)
               window.close();

           if (event.type == sf::Event::KeyPressed)
           {
               if (event.key.code == Keyboard::Left)
               {
                   cx--;
                   if (check_block() == false) cx++;
               }
               else if (event.key.code == Keyboard::Right)
               {
                   cx++;
                   if (check_block() == false) cx--;
               }
               else if (event.key.code == Keyboard::Down)
               {
                   go_down();
               }
               else if (event.key.code == Keyboard::Up)
               {
                   rotate();
                   if (check_block() == false)
                   {
                       rotate();
                       rotate();
                       rotate();
                   }
               }
               else if (event.key.code == Keyboard::Space)
               {
                   while (go_down());
               }
               else if (event.key.code == Keyboard::R)
               {
                   if (game_over)
                   {
                       restart_game();
                   }
                   
               }
               else if (event.key.code == Keyboard::M)
               {
                   if (!sound_off)
                   {
                       song.play();
                       sound_off = true;
                   }
                   else
                   {
                       song.stop();
                       sound_off = false;
                   }
               }
           }
       }

       window.clear();

       auto draw_board = [&]()
       {
           for (int y = 0; y < board_h; y++)
               for(int x = 0; x < board_w; x++)
                   if (board[y][x])
                   {
                       cell.setPosition(Vector2f(x * cell_size * 2, y * cell_size * 2));
                       window.draw(cell);
                   }
       };

       auto check_board = [&]()
       {
           int to = board_h - 1; //Place where we move the pieces 

           int ttrs = 0;

           for (int from = board_h - 1; from >= 0; from--)
           {
               int crnt = 0;

               for (int x = 0; x < board_w; x++)
                   if (board[from][x])
                   {
                       crnt++;
                   }

               if (crnt < board_w)
               {
                   for (int x = 0; x < board_w; x++)
                       board[to][x] = board[from][x];
                   to--;

               }
               else
               {
                   Score++;
                   ttrs++;
               }
               if (ttrs == 4)
               {
                   Score++;
               }

           }
           
       };

       cur_time = clock.getElapsedTime();

       fps = 1.f / (cur_time.asSeconds() - prev_time.asSeconds());

       prev_time = cur_time;

       window.draw(bckgrnd);
       window.draw(scoreb);

       ss1.str("");
       ss2.str("");

       ss1 << "Score: " << Score;
       ss2 << "Fps: " << floor(fps);

       score_t.setString(ss1.str());
       score_t.setPosition(board_w * cell_size * 2 + 50, 250);
       score_t.setFillColor(Color::Magenta);
       score_t.setCharacterSize(52.f);
       score_t.setLetterSpacing(1.25f);

       fps_t.setString(ss2.str());
       fps_t.setPosition(board_w * cell_size * 2 + 230, 10);
       fps_t.setFillColor(Color::Magenta);
       fps_t.setCharacterSize(20.f);
       fps_t.setLetterSpacing(1.f);

       window.draw(score_t);
       window.draw(fps_t);

       draw_board();
       check_board();

       auto draw_block = [&]()
       {
           cell.setTextureRect(IntRect(kind * 18, 0, 18, 18));
           for (int y = 0;  y < 4; y++)
               for(int x = 0; x < 4; x++)
                   if (Shapes[kind][y][x])
                   {
                       cell.setPosition(Vector2f((cx + x)* cell_size * 2, (cy + y)* cell_size * 2));
                       window.draw(cell);
                   }
       };

       draw_block();

       end_game();
       
       window.display();
         
   }

   return 0;
}