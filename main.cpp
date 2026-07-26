#include <raylib.h>
#include <iostream>
#include <cctype>
#include "board.h"
using namespace std;

const int windowWidth = 800;
const int windowHeight = 800;

Board board;
Rectangle boardPos[8][8];
bool initialSetup = true;
int selectedPiece[2] = {-1, -1};
bool skipNextPieceCheck = false;

Texture2D b_bishop;
Texture2D b_king;
Texture2D b_knight;
Texture2D b_pawn;
Texture2D b_queen;
Texture2D b_rook;
Texture2D w_bishop;
Texture2D w_king;
Texture2D w_knight;
Texture2D w_pawn;
Texture2D w_queen;
Texture2D w_rook;

void DrawBoard() {
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            float xPos = file * 100;
            float yPos = rank * 100;

            // Alternate colors based on square position
            Color squareColor = ((file + rank) % 2 == 0) ? Color{118, 153, 174, 255} : Color{212, 223, 229, 255};
            Color oppositeColor = ((file + rank) % 2 == 1) ? Color{118, 153, 174, 255} : Color{212, 223, 229, 255};
            
            // Draw Square
            if (selectedPiece[0] == rank && selectedPiece[1] == file) { // Highlight selected square
                DrawRectangle(xPos, yPos, 100, 100, Color{149, 217, 233, 255});
            } else {
                DrawRectangle(xPos, yPos, 100, 100, squareColor);
            }
            
            if (initialSetup) {
                boardPos[rank][file] = Rectangle{xPos, yPos, 100, 100};
            }

            // Draw Rank/File Label
            if (rank == 7) { // Draw file letters at bottom left corner of each square
                string files = "abcdefgh";
                DrawText(TextFormat("%c", files[file]), xPos + 80, yPos + 80, 18, oppositeColor);
            }
            if (file == 0) { // Draw rank numbers at top right corner of each square
                DrawText(TextFormat("%d", 8 - rank), xPos + 8, yPos + 8, 18, oppositeColor);
            }

            // Draw Pieces
            Rectangle srcRec = { 0.0f, 0.0f, 100.0f, 100.0f };
            Rectangle destRec = { xPos, yPos, 100.0f, 100.0f };
            Vector2 origin = { 0.0f, 0.0f };

            switch (board.state[rank][file]) {
                case 'p': DrawTexturePro(b_pawn,   srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'r': DrawTexturePro(b_rook,   srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'n': DrawTexturePro(b_knight, srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'b': DrawTexturePro(b_bishop, srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'q': DrawTexturePro(b_queen,  srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'k': DrawTexturePro(b_king,   srcRec, destRec, origin, 0.0f, WHITE); break;

                case 'P': DrawTexturePro(w_pawn,   srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'R': DrawTexturePro(w_rook,   srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'N': DrawTexturePro(w_knight, srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'B': DrawTexturePro(w_bishop, srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'Q': DrawTexturePro(w_queen,  srcRec, destRec, origin, 0.0f, WHITE); break;
                case 'K': DrawTexturePro(w_king,   srcRec, destRec, origin, 0.0f, WHITE); break;
                default: break;
            }
        }
    }
    initialSetup = false;
}

void PieceCheck() {
    if (skipNextPieceCheck) {
        skipNextPieceCheck = false;
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Vector2 mousePos = GetMousePosition();
    bool hasSelectedPiece = (selectedPiece[0] != -1 && selectedPiece[1] != -1);

    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            if (CheckCollisionPointRec(mousePos, boardPos[rank][file])) {
                char clickedSquare = board.state[rank][file];

                // Select a piece if no square has been selected yet
                if (!hasSelectedPiece) {
                    if (clickedSquare != ' ') {
                        selectedPiece[0] = rank;
                        selectedPiece[1] = file;
                    }
                    return;
                }

                char selectedSquare = board.state[selectedPiece[0]][selectedPiece[1]];

                // Clicking the same piece will deselect it
                if (rank == selectedPiece[0] && file == selectedPiece[1]) {
                    selectedPiece[0] = -1;
                    selectedPiece[1] = -1;
                    return;
                }

                
                if (
                    (clickedSquare == ' ' && selectedSquare != ' ') // Moves a piece to an empty square
                 || (clickedSquare != ' ' && isupper(clickedSquare) != isupper(selectedSquare)) // Moves a selected piece to an enemy piece (capture)
                ) {
                    board.state[rank][file] = selectedSquare;
                    board.state[selectedPiece[0]][selectedPiece[1]] = ' ';
                    skipNextPieceCheck = true;
                }

                // Switches between same team's piece
                if (clickedSquare != ' ' && isupper(clickedSquare) == isupper(selectedSquare)) {
                    selectedPiece[0] = rank;
                    selectedPiece[1] = file;
                }

                if (skipNextPieceCheck) {
                    selectedPiece[0] = -1;
                    selectedPiece[1] = -1;
                    return;
                }
            }
        }
    }
}

int main() {
    InitWindow(windowWidth, windowHeight, "Chess Engine");
    SetTargetFPS(60);

    // Load images into RAM
    Image i_b_bishop = LoadImage("../assets/black-bishop.png");
    Image i_b_king = LoadImage("../assets/black-king.png");
    Image i_b_knight = LoadImage("../assets/black-knight.png");
    Image i_b_pawn = LoadImage("../assets/black-pawn.png");
    Image i_b_queen = LoadImage("../assets/black-queen.png");
    Image i_b_rook = LoadImage("../assets/black-rook.png");

    Image i_w_bishop = LoadImage("../assets/white-bishop.png");
    Image i_w_king = LoadImage("../assets/white-king.png");
    Image i_w_knight = LoadImage("../assets/white-knight.png");
    Image i_w_pawn = LoadImage("../assets/white-pawn.png");
    Image i_w_queen = LoadImage("../assets/white-queen.png");
    Image i_w_rook = LoadImage("../assets/white-rook.png");

    // Initialize textures and free image RAM
    b_bishop = LoadTextureFromImage(i_b_bishop); UnloadImage(i_b_bishop);
    b_king   = LoadTextureFromImage(i_b_king);   UnloadImage(i_b_king);
    b_knight = LoadTextureFromImage(i_b_knight); UnloadImage(i_b_knight);
    b_pawn   = LoadTextureFromImage(i_b_pawn);   UnloadImage(i_b_pawn);
    b_queen  = LoadTextureFromImage(i_b_queen);  UnloadImage(i_b_queen);
    b_rook   = LoadTextureFromImage(i_b_rook);   UnloadImage(i_b_rook);

    w_bishop = LoadTextureFromImage(i_w_bishop); UnloadImage(i_w_bishop);
    w_king   = LoadTextureFromImage(i_w_king);   UnloadImage(i_w_king);
    w_knight = LoadTextureFromImage(i_w_knight); UnloadImage(i_w_knight);
    w_pawn   = LoadTextureFromImage(i_w_pawn);   UnloadImage(i_w_pawn);
    w_queen  = LoadTextureFromImage(i_w_queen);  UnloadImage(i_w_queen);
    w_rook   = LoadTextureFromImage(i_w_rook);   UnloadImage(i_w_rook);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawBoard();
        PieceCheck();

        EndDrawing();
    }

    // Clean up VRAM on closing
    UnloadTexture(b_bishop);
    UnloadTexture(b_king);
    UnloadTexture(b_knight);
    UnloadTexture(b_pawn);
    UnloadTexture(b_queen);
    UnloadTexture(b_rook);

    UnloadTexture(w_bishop);
    UnloadTexture(w_king);
    UnloadTexture(w_knight);
    UnloadTexture(w_pawn);
    UnloadTexture(w_queen);
    UnloadTexture(w_rook);

    CloseWindow();
    return 0;
}