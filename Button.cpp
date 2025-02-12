#include "Button.h"

Button::Button(float x, float y, float width, float height, Font& font, string buttonText){
    // RECTANGLE
    rect.setPosition(Vector2f(x,y));
    rect.setSize(Vector2f(width, height));
    rect.setFillColor(Color::White);

    // CIRCLES
    radius = height/2;
    // LEFT CIRCLE
    setCircleDimensions(left_circle, x-radius, y);
    // RIGHT CIRCLE
    setCircleDimensions(right_circle, x-radius+width, y);

    text.setFont(font);
    text.setString(buttonText);
    text.setCharacterSize(24);
    text.setFillColor(Color::Black);

    FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top  + textBounds.height / 2.f);
    text.setPosition(rect.getPosition() + (rect.getSize() / 2.f));

    select(false);
}

void Button::setCircleDimensions(CircleShape& circle, float x, float y) {
    circle.setPosition(Vector2f(x, y));
    circle.setRadius(radius);
}

    // METHODS
bool Button::isClicked(Vector2f mousePos){
    bool isOnButton = rect.getGlobalBounds().contains(mousePos) ||
                        left_circle.getGlobalBounds().contains(mousePos) ||
                        right_circle.getGlobalBounds().contains(mousePos);
    return isOnButton;
}

bool Button::isHighlighted(Vector2f mousePos){
    return isClicked(mousePos);
}

bool Button::isSelected() {
    return selected;
}

string Button::getText() {
    return text.getString();
}

void Button::centerText() {
    FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top  + textBounds.height / 2.f);
    text.setPosition(rect.getPosition() + (rect.getSize() / 2.f));
}

void Button::setText(string _text) {
    text.setString(_text);
	centerText();
}

void Button::setTextColor(Color color) {
    text.setColor(color);
}

void Button::setCharSize(unsigned int char_size) {
    text.setCharacterSize(char_size);
}

void Button::select(bool _selected) {
    selected = _selected;
}

void Button::draw(RenderWindow& app){
    app.draw(rect);
    app.draw(left_circle);
    app.draw(right_circle);
    app.draw(text);
}

void Button::isCorrect(bool correct){
    if(correct){
        rect.setFillColor(GREEN);
        left_circle.setFillColor(GREEN);
        right_circle.setFillColor(GREEN);
    }
    else{
        rect.setFillColor(RED);
        left_circle.setFillColor(RED);
        right_circle.setFillColor(RED);
    }
}

void Button::resetColor(){
    rect.setFillColor(Color::White);
    left_circle.setFillColor(Color::White);
    right_circle.setFillColor(Color::White);
}

void Button::setColor(Color color) {
    rect.setFillColor(color);
    left_circle.setFillColor(color);
    right_circle.setFillColor(color);
}

void Button::deselect(){
    rect.setFillColor(GREY);
    left_circle.setFillColor(GREY);
    right_circle.setFillColor(GREY);
}
