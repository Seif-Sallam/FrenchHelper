#include "raylib.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <unordered_map>

std::string
toLower(std::string w)
{
	for (auto& c : w)
		c = tolower(c);
	return w;
}

std::vector<std::string>
read_file(const char* filename)
{
	std::ifstream file(filename);
	if (file.is_open() == false)
	{
		std::cerr << "Failed to open the file: " << filename;
		exit(1);
	}

	std::vector<std::string> v{};
	int i = 1;
	while(file.eof() == false)
	{
		std::string word{};

		std::getline(file, word);
		if (word.length() < 1)
			break;

		v.push_back(word);
		++i;
	}

	file.close();
	return v;
}


int main(void)
{
	const char* filename = RESOURCES_DIR "/words.txt";
	auto words = read_file(filename);

	const int screenWidth = 800;
	const int screenHeight = 450;

	SetTraceLogLevel(LOG_NONE);

	InitWindow(screenWidth, screenHeight, "French Helper");

	SetTargetFPS(20);

	Image image = LoadImage(RESOURCES_DIR"/french_keyboard.png");
	auto font = LoadFont(RESOURCES_DIR"./Lancelot-Regular.ttf");

	float aspect_ratio = float(image.width) / float(image.height);

	int width = 3 * screenWidth / 5;
	ImageResize(&image, width, int(width / aspect_ratio));
	Texture2D texture = LoadTextureFromImage(image);

	std::vector<std::string> writtenWords;
	std::vector<std::string> other;

	char writtenWord[255] = {};
	int letterCount = 0;

	int currentWord = 0;

	const float minHeight = screenHeight / 2.0f;
	const float wordHeight = 30.0;
	const float minWidth = screenWidth / 3.0f;

	int fontSpacing = 1;
	int fontSize = 30;

	float wordWidth = 100 + minWidth;

	while (!WindowShouldClose())
	{
		// Get char pressed (unicode character) on the queue
		int key = GetCharPressed();

		// Check if more characters have been pressed on the same frame
		while (key > 0)
		{
			// NOTE: Only allow keys in range [32..125]
			if ((key >= 32) && (key <= 125) && letterCount < 254)
			{
				writtenWord[letterCount] = (char)key;
				writtenWord[letterCount+1] = '\0'; // Add null terminator at the end of the string.
				letterCount++;
			}

			key = GetCharPressed();  // Check next character in the queue
		}

		if (IsKeyPressed(KEY_BACKSPACE))
		{
			letterCount--;
			if (letterCount < 0) letterCount = 0;
			writtenWord[letterCount] = '\0';
		}
		if (IsKeyPressed(KEY_ENTER))
		{
			if (letterCount == 0)
				continue;
			if (currentWord == words.size() - 1)
				continue;
			currentWord = (currentWord + 1) % words.size();
			writtenWords.push_back(writtenWord);
			for (int i = 0; i < 255; ++i)
				writtenWord[i] = 0;
			letterCount = 0;
		}

		BeginDrawing();

		ClearBackground(RAYWHITE);

		// DrawTexture(texture, screenWidth - width, screenHeight / 3, WHITE);

		for (int i = 0; i <= words.size(); ++i)
		{
			DrawTextEx(font, std::to_string(i + 1).c_str(), Vector2{minWidth, minHeight + i * wordHeight - wordHeight * currentWord}, fontSize, fontSpacing, BLACK);
		}

		for (int i = 0; i < writtenWords.size(); ++i) {
			Vector2 pos = Vector2{float(wordWidth), (float)minHeight + i * wordHeight - wordHeight * currentWord};
			Color color = RED;
			if (toLower(writtenWords[i]) == toLower(words[i]))
				color = GREEN;
			DrawTextEx(font, writtenWords[i].c_str(), pos, float(fontSize), float(fontSpacing), color);
		}

		DrawTextEx(font, writtenWord, Vector2{(float)wordWidth, (float)minHeight}, fontSize, fontSpacing, RED);
		static int shouldBlink = 0;

		if (shouldBlink % 10 < 7)
		{
			auto fontStuff = MeasureTextEx(font, writtenWord, (float)fontSize, fontSpacing);
			DrawRectangle(wordWidth + fontStuff.x, minHeight , 2, (int)fontStuff.y + fontSpacing, BLACK);
		}
		shouldBlink++;

		for (int i = 0; i < currentWord; ++i)
		{
			DrawTextEx(font, words[i].c_str(), Vector2{float(wordWidth + 200), minHeight + i * wordHeight - wordHeight * currentWord}, fontSize, fontSpacing, GREEN);
		}
		EndDrawing();
	}
	UnloadImage(image);
	UnloadTexture(texture);
	CloseWindow();
	return 0;
}