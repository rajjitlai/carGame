/**
 * @file   CarGame.cpp
 * @brief  Terminal-based car dodging game for Windows with stats persistence.
 * @author Rajjit Laishram
 * @date   Original: ~2022 | Refactored & Optimized: 2026
 *
 * Build:
 *   g++ -std=c++17 -O2 -o CarGame.exe CarGame.cpp
 *   Or double-click build.bat
 *
 * Controls (in-game):
 *   A / Left  – Move car left
 *   D / Right – Move car right
 *   ESC       – Quit to menu
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <string>
#include <algorithm>

// ─── Constants ────────────────────────────────────────────────────────────────

constexpr int SCREEN_WIDTH  = 90;
constexpr int SCREEN_HEIGHT = 26;
constexpr int WIN_WIDTH     = 70;
constexpr int BORDER_COLS   = 17;
constexpr int ENEMY_COUNT   = 3;
constexpr int CAR_SIZE      = 4;
constexpr int CAR_BOTTOM_Y  = 22;   // row where player car sits
constexpr int ROAD_MIN_X    = BORDER_COLS + 1;
constexpr int ROAD_MAX_X    = WIN_WIDTH - BORDER_COLS - CAR_SIZE - 1;
constexpr const char* DATA_FILE = "data.txt";

// ─── Difficulty table: {min score, sleep ms, level label} ────────────────────

struct DifficultyLevel {
    int  minScore;
    DWORD sleepMs;
    int  level;
};

constexpr DifficultyLevel DIFFICULTY_TABLE[] = {
    { 20, 18, 5 },
    { 15, 28, 4 },
    { 10, 40, 3 },
    {  5, 55, 2 },
    {  0, 70, 1 },
};

// ─── Utility: Console helpers ─────────────────────────────────────────────────

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void gotoxy(int x, int y)
{
    COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hConsole, pos);
}

void setCursor(bool visible, DWORD size = 20)
{
    CONSOLE_CURSOR_INFO ci;
    ci.bVisible = visible;
    ci.dwSize   = (size == 0) ? 20 : size;
    SetConsoleCursorInfo(hConsole, &ci);
}

/// Return today's date as "YYYY-MM-DD"
std::string todayDate()
{
    time_t now = time(nullptr);
    char buf[11];
#ifdef _MSC_VER
    tm local{};
    localtime_s(&local, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &local);
#else
    // MinGW / GCC: localtime is safe in this single-threaded context
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
#endif
    return buf;
}

// ─── CarGame ──────────────────────────────────────────────────────────────────

class CarGame
{
public:
    CarGame()
    {
        srand(static_cast<unsigned>(time(nullptr)));
    }

    void run()
    {
        setCursor(false, 0);
        char choice;
        do {
            showMenu();
            choice = static_cast<char>(_getche());
            switch (choice) {
                case '1': showInstructions(); break;
                case '2': play();             break;
                case '3': showStats();        break;
                case '4': exit(0);            break;
            }
        } while (true);
    }

private:
    // ── State ─────────────────────────────────────────────────────────────────
    int  m_enemyX[ENEMY_COUNT]      = {};
    int  m_enemyY[ENEMY_COUNT]      = {};
    bool m_enemyActive[ENEMY_COUNT] = {};
    int  m_carPos   = WIN_WIDTH / 2;
    int  m_score    = 0;
    int  m_highScore = 0;   // persists within a session

    const char m_car[CAR_SIZE][CAR_SIZE] = {
        {' ', '+', '+', ' '},
        {'+', '+', '+', '+'},
        {' ', '+', '+', ' '},
        {'+', '+', '+', '+'}
    };

    // ── Difficulty helpers ────────────────────────────────────────────────────

    const DifficultyLevel& currentDifficulty() const
    {
        for (const auto& d : DIFFICULTY_TABLE)
            if (m_score >= d.minScore)
                return d;
        return DIFFICULTY_TABLE[4]; // level 1 fallback
    }

    // ── UI ────────────────────────────────────────────────────────────────────

    void showMenu()
    {
        system("cls");
        gotoxy(10, 5);  std::cout << " --------------------";
        gotoxy(10, 6);  std::cout << " |     CAR GAME     |";
        gotoxy(10, 7);  std::cout << " --------------------";
        gotoxy(10, 8);  std::cout << "1. Instructions";
        gotoxy(10, 9);  std::cout << "2. Start Game";
        gotoxy(10, 10); std::cout << "3. Stats";
        gotoxy(10, 11); std::cout << "4. Quit";
        gotoxy(10, 13); std::cout << "Select Option: ";
    }

    void showInstructions()
    {
        system("cls");
        std::cout << "Instructions:\n"
                  << "--------------------\n"
                  << " Dodge enemy cars by moving left or right.\n\n"
                  << " Press 'A' to move left\n\n"
                  << " Press 'D' to move right\n\n"
                  << " Press 'ESC' to quit to menu\n\n"
                  << " Speed increases every 5 points — survive as long as you can!\n\n"
                  << "Press any key to go back to menu.";
        _getch();
    }

    // ── Stats Screen ──────────────────────────────────────────────────────────

    void showStats()
    {
        system("cls");
        std::cout << "\t\t===================================\n"
                  << "\t\t          GAME STATISTICS          \n"
                  << "\t\t===================================\n\n";

        std::ifstream file(DATA_FILE);
        if (!file.is_open()) {
            std::cout << "\t\tNo games played yet.\n"
                      << "\t\tPlay a round first!\n";
        } else {
            // Print header
            std::cout << "  " << std::left
                      << std::setw(12) << "Date"
                      << std::setw(10) << "Score"
                      << std::setw(14) << "High Score"
                      << std::setw(8)  << "Level"
                      << "\n";
            std::cout << "  " << std::string(42, '-') << "\n";

            std::string line;
            int totalGames = 0;
            int bestEver   = 0;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::cout << "  " << line << "\n";
                ++totalGames;

                // Parse the score field (second column) to find all-time best
                std::istringstream ss(line);
                std::string date, scoreStr, hiStr, levelStr;
                ss >> date >> scoreStr >> hiStr >> levelStr;
                try {
                    bestEver = std::max(bestEver, std::stoi(hiStr));
                } catch (...) {}
            }
            file.close();

            std::cout << "\n  Total games played : " << totalGames << "\n"
                      << "  All-time high score: " << bestEver    << "\n";
        }

        std::cout << "\n\nPress any key to go back to menu.";
        _getch();
    }

    // ── Data persistence ──────────────────────────────────────────────────────

    void saveSession(int score, int highScore, int level)
    {
        std::ofstream file(DATA_FILE, std::ios::app);
        if (!file.is_open()) return;

        file << std::left
             << std::setw(12) << todayDate()
             << std::setw(10) << score
             << std::setw(14) << highScore
             << std::setw(8)  << level
             << "\n";
    }

    // ── Sidebar ───────────────────────────────────────────────────────────────

    void drawSidebar()
    {
        gotoxy(WIN_WIDTH + 7, 2);  std::cout << "CAR GAME";
        gotoxy(WIN_WIDTH + 6, 4);  std::cout << "----------";
        gotoxy(WIN_WIDTH + 7, 12); std::cout << "Controls";
        gotoxy(WIN_WIDTH + 7, 13); std::cout << "---------";
        gotoxy(WIN_WIDTH + 2, 14); std::cout << " A key  - Left";
        gotoxy(WIN_WIDTH + 2, 15); std::cout << " D key  - Right";
        gotoxy(WIN_WIDTH + 2, 16); std::cout << " ESC    - Quit";
    }

    void updateSidebarStats()
    {
        const auto& d = currentDifficulty();
        gotoxy(WIN_WIDTH + 7, 5);
        std::cout << "Score    : " << m_score << "   ";
        gotoxy(WIN_WIDTH + 7, 6);
        std::cout << "Best     : " << m_highScore << "   ";
        gotoxy(WIN_WIDTH + 7, 7);
        std::cout << "Speed Lv : " << d.level << "   ";
    }

    // ── Border ────────────────────────────────────────────────────────────────

    void drawBorder()
    {
        for (int row = 0; row < SCREEN_HEIGHT; ++row) {
            for (int col = 0; col < BORDER_COLS; ++col) {
                gotoxy(col,              row); std::cout << '+';
                gotoxy(WIN_WIDTH - col,  row); std::cout << '+';
            }
            gotoxy(SCREEN_WIDTH, row); std::cout << '+';
        }
    }

    // ── Car ───────────────────────────────────────────────────────────────────

    void drawCar()
    {
        for (int row = 0; row < CAR_SIZE; ++row) {
            gotoxy(m_carPos, row + CAR_BOTTOM_Y);
            for (int col = 0; col < CAR_SIZE; ++col)
                std::cout << m_car[row][col];
        }
    }

    void eraseCar()
    {
        for (int row = 0; row < CAR_SIZE; ++row) {
            gotoxy(m_carPos, row + CAR_BOTTOM_Y);
            std::cout << "    ";
        }
    }

    // ── Enemies ───────────────────────────────────────────────────────────────

    /// Spawn enemy at a random X that doesn't conflict with other active enemies.
    void spawnEnemy(int i)
    {
        int attempts = 0;
        int newX;
        do {
            newX = ROAD_MIN_X + rand() % 33;
            bool conflict = false;
            for (int j = 0; j < ENEMY_COUNT; ++j) {
                if (j == i || !m_enemyActive[j]) continue;
                if (std::abs(newX - m_enemyX[j]) < CAR_SIZE + 2) {
                    conflict = true;
                    break;
                }
            }
            if (!conflict) break;
        } while (++attempts < 20); // give up after 20 tries to avoid inf loop
        m_enemyX[i] = newX;
    }

    void drawEnemy(int i)
    {
        if (!m_enemyActive[i]) return;
        gotoxy(m_enemyX[i], m_enemyY[i]);     std::cout << "****";
        gotoxy(m_enemyX[i], m_enemyY[i] + 1); std::cout << " ** ";
        gotoxy(m_enemyX[i], m_enemyY[i] + 2); std::cout << "****";
        gotoxy(m_enemyX[i], m_enemyY[i] + 3); std::cout << " ** ";
    }

    void eraseEnemy(int i)
    {
        if (!m_enemyActive[i]) return;
        for (int row = 0; row < CAR_SIZE; ++row) {
            gotoxy(m_enemyX[i], m_enemyY[i] + row);
            std::cout << "     ";
        }
    }

    void resetEnemy(int i)
    {
        eraseEnemy(i);
        m_enemyY[i] = 1;
        spawnEnemy(i);
    }

    // ── Collision ─────────────────────────────────────────────────────────────

    /// Check ALL active enemies against the player car.
    bool checkCollision()
    {
        for (int i = 0; i < ENEMY_COUNT; ++i) {
            if (!m_enemyActive[i]) continue;
            if (m_enemyY[i] + CAR_SIZE >= CAR_BOTTOM_Y) {
                int overlap = m_enemyX[i] + CAR_SIZE - m_carPos;
                if (overlap >= 0 && overlap < 9)
                    return true;
            }
        }
        return false;
    }

    // ── Game Over ─────────────────────────────────────────────────────────────

    void showGameOver(int level)
    {
        saveSession(m_score, m_highScore, level);

        system("cls");
        std::cout << "\n"
                  << "\t\t---------------------------------\n"
                  << "\t\t---------- Game Over :(----------\n"
                  << "\t\t---------------------------------\n\n"
                  << "\t\tScore        : " << m_score     << "\n"
                  << "\t\tHigh Score   : " << m_highScore << "\n"
                  << "\t\tSpeed Level  : " << level       << "\n\n"
                  << "\t\tResults saved to data.txt\n\n"
                  << "\t\tPress any key to go back to menu.";
        _getch();
    }

    // ── Game Loop ─────────────────────────────────────────────────────────────

    void play()
    {
        // Reset per-round state
        m_carPos = WIN_WIDTH / 2 - 1;
        m_score  = 0;
        for (int i = 0; i < ENEMY_COUNT; ++i) {
            m_enemyActive[i] = false;
            m_enemyY[i]      = 1;
        }
        m_enemyActive[0] = true; // first enemy always active from the start

        system("cls");
        drawBorder();
        drawSidebar();
        updateSidebarStats();

        for (int i = 0; i < ENEMY_COUNT; ++i)
            spawnEnemy(i);

        gotoxy(18, 5);
        std::cout << "Press any key to start :)";
        _getch();
        gotoxy(18, 5);
        std::cout << "                         ";

        while (true) {
            // ── Input ────────────────────────────────────────────────────────
            if (_kbhit()) {
                char ch = static_cast<char>(_getch());
                if (ch == 'a' || ch == 'A') {
                    if (m_carPos > ROAD_MIN_X) m_carPos -= 4;
                } else if (ch == 'd' || ch == 'D') {
                    if (m_carPos < ROAD_MAX_X) m_carPos += 4;
                } else if (ch == 27) {
                    break;
                }
            }

            // ── Draw frame ───────────────────────────────────────────────────
            drawCar();
            for (int i = 0; i < ENEMY_COUNT; ++i) drawEnemy(i);

            if (checkCollision()) {
                showGameOver(currentDifficulty().level);
                return;
            }

            Sleep(currentDifficulty().sleepMs);

            // ── Erase for next frame ──────────────────────────────────────────
            eraseCar();
            for (int i = 0; i < ENEMY_COUNT; ++i) eraseEnemy(i);

            // ── Enemy activation thresholds ───────────────────────────────────
            // Enemy 1: when enemy 0 is midway
            if (m_enemyY[0] == 10 && !m_enemyActive[1])
                m_enemyActive[1] = true;

            // Enemy 2: when score reaches 5
            if (m_score >= 5 && !m_enemyActive[2]) {
                m_enemyActive[2] = true;
                m_enemyY[2]      = 1;
            }

            // ── Advance enemies ───────────────────────────────────────────────
            for (int i = 0; i < ENEMY_COUNT; ++i)
                if (m_enemyActive[i]) ++m_enemyY[i];

            // ── Score & reset enemies that exit the bottom ────────────────────
            for (int i = 0; i < ENEMY_COUNT; ++i) {
                if (m_enemyActive[i] && m_enemyY[i] > SCREEN_HEIGHT - CAR_SIZE) {
                    resetEnemy(i);
                    ++m_score;
                    if (m_score > m_highScore) m_highScore = m_score;
                    updateSidebarStats();
                }
            }
        }
    }
};

// ─── Entry Point ─────────────────────────────────────────────────────────────

int main()
{
    CarGame game;
    game.run();
    return 0;
}
