#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Vector2.hpp"
#include "Line.hpp"

using namespace shape;

class Debug
{
public:
   /// <summary>
   /// Logs a message in the console.
   /// </summary>
   inline static void Log(const std::string& message) {
       std::cout << message << std::endl;
   }

   /// <summary>
   /// Draws all debug drawings to the screen.
   /// </summary>
   inline static void Draw() {
       for (const auto& drawing : drawings) {
		   Game::GetInstance()->GetWindow()->draw(*drawing);
	   }
       for (auto* drawable : drawings) {
           delete drawable;
       }
	   drawings.clear();
   }

   /// <summary>
   /// Draws all debug drawings to the screen and displays the window.
   /// Use of this function is highly discuraged.
   /// </summary>
   inline static void DrawImmidiate() {
       Game::GetInstance()->GetWindow()->clear();
       for (const auto& drawing : drawings) {
           Game::GetInstance()->GetWindow()->draw(*drawing);
       }
       Game::GetInstance()->GetWindow()->display();
       for (auto* drawable : drawings) {
           delete drawable;
       }
       drawings.clear();
   }

   /// <summary>
   /// Draws a line between two points with the specified color.
   /// </summary>
   inline static void DrawLine(const Vector2& p1, const Vector2& p2, sf::Color color = sf::Color::White) {
       sf::VertexArray* line = new sf::VertexArray(sf::Lines, 2);
       (*line)[0] = sf::Vertex(sf::Vector2f(p1.x, p1.y), color);
       (*line)[1] = sf::Vertex(sf::Vector2f(p2.x, p2.y), color);
       drawings.push_back(line);
   }  
   
   /// <summary>
   /// Draws a line between two points with the specified color.
   /// </summary>
   inline static void DrawLine(const Line& l, sf::Color color = sf::Color::White) {
       sf::VertexArray* line = new sf::VertexArray(sf::Lines, 2);
       (*line)[0] = sf::Vertex(sf::Vector2f(l.start.x, l.start.y), color);
       (*line)[1] = sf::Vertex(sf::Vector2f(l.end.x, l.end.y), color);
       drawings.push_back(line);
   }

   /// <summary>
   /// Draws a continuous line from all points in the vector with the specified color.
   /// </summary>
   inline static void DrawLineList(std::vector<Vector2>* line, sf::Color color = sf::Color::White) {
       sf::VertexArray* lineArray = new sf::VertexArray(sf::LineStrip, line->size());
       for (int i = 0; i < line->size(); i++) {
           (*lineArray)[i] = sf::Vertex(sf::Vector2f((*line)[i].x, (*line)[i].y), color);
       }
       drawings.push_back(lineArray);
   }

   /// <summary>
   /// Draws all lines in line with the specified color.
   /// </summary>
   inline static void DrawLineList(std::vector<Line>* line, sf::Color color = sf::Color::White) {
       sf::VertexArray* lineArray = new sf::VertexArray(sf::Lines, line->size() * 2);
       for (int i = 0; i < line->size(); i += 2) {
           (*lineArray)[i] = sf::Vertex(sf::Vector2f((*line)[i].start.x, (*line)[i].start.y), color);
		   (*lineArray)[i + 1] = sf::Vertex(sf::Vector2f((*line)[i].end.x, (*line)[i].end.y), color);
       }
       drawings.push_back(lineArray);
   }

   /// <summary>
   /// Draws the text in the specified position.
   /// </summary>
   inline static void DrawText(std::string text, Vector2 position, int fontSize=14, sf::Color color = sf::Color::White) {
       // Load the Arial font from the assets folder
       if (font == nullptr) {
           font = new sf::Font();
           if (!font->loadFromFile("assets/fonts/Arial.ttf")) {
               Log("Error loading Arial font from assets/fonts/Arial.ttf");
               return;
           }
       }

	   sf::Text* textObj = new sf::Text();
       textObj->setFont(*font);
	   textObj->setString(text);
	   textObj->setPosition(position.x, position.y);
	   textObj->setFillColor(color);
	   textObj->setCharacterSize(fontSize);
       drawings.push_back(textObj);
   }

   /// <summary>
   /// Draws a rectangle with the specified color.
   /// </summary>
   inline static void DrawRect(Rect rect, sf::Color color = sf::Color::White) {
	   sf::VertexArray* line = new sf::VertexArray(sf::Quads, 4);
	   (*line)[0] = sf::Vertex(sf::Vector2f(rect.min.x, rect.min.y), color);
	   (*line)[1] = sf::Vertex(sf::Vector2f(rect.max.x, rect.min.y), color);
	   (*line)[2] = sf::Vertex(sf::Vector2f(rect.max.x, rect.max.y), color);
	   (*line)[3] = sf::Vertex(sf::Vector2f(rect.min.x, rect.max.y), color);
	   drawings.push_back(line);
   }

   /// <summary>
   /// Draws a rectangle outline with the specified color.
   /// </summary>
   inline static void DrawWireRect(Rect rect, sf::Color color = sf::Color::White) {
	   sf::VertexArray* line = new sf::VertexArray(sf::LineStrip, 5);
	   (*line)[0] = sf::Vertex(sf::Vector2f(rect.min.x, rect.min.y), color);
	   (*line)[1] = sf::Vertex(sf::Vector2f(rect.max.x, rect.min.y), color);
	   (*line)[2] = sf::Vertex(sf::Vector2f(rect.max.x, rect.max.y), color);
	   (*line)[3] = sf::Vertex(sf::Vector2f(rect.min.x, rect.max.y), color);
	   (*line)[4] = sf::Vertex(sf::Vector2f(rect.min.x, rect.min.y), color);
	   drawings.push_back(line);
   }

private:
    inline static std::vector<sf::Drawable*> drawings;
    inline static sf::Font* font;
};
