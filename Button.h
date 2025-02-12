#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

// CUSTOM COLORS
const Color GREY = Color(200, 200, 200);
const Color GREEN = Color(0, 119, 73);      // SA FLAG GREEN
const Color RED = Color(224, 60, 49);       // SA FLAG RED

class Button {
private:
    RectangleShape rect;
    CircleShape left_circle;
    CircleShape right_circle;
    float radius;
    bool selected;
    Text text;

    // PRIVATE METHOD
    void setCircleDimensions(CircleShape& circle, float x, float y);

public:
    // CONSTRUCTOR
    Button(float x, float y, float width, float height, Font& font, string buttonText);

    // METHODS
    bool isClicked(Vector2f mousePos);
    bool isHighlighted(Vector2f mousePos);
    bool isSelected();
    // SETTER FOR SELECTED
    void select(bool _selected);
    string getText();
    void centerText();
    void setText(string _text);
    void setTextColor(Color color);
    void setCharSize(unsigned int char_size);
    void draw(RenderWindow& app);
    void isCorrect(bool correct);
    void resetColor();
    void setColor(Color color);
    void deselect();
};

#endif // BUTTON_H_INCLUDED
