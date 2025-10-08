#include "Button.h"
#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>  // Also required if you're working with `sf::RenderWindow`
#include <SFML/Audio.hpp>
#include <stdexcept>


Button::Button(sf::Vector2f textCenterPos, sf::Font font, std::string txt, int fontSize, 
	sf::Color color, sf::Color colorOnHover, float rectAmplifyingValue) : text(font, ""){
	this->textCenterPos = textCenterPos;
	this->font = font;
	this->text.setFont(this->font);

	this->textString = txt;
	this->text.setString(textString);
	this->text.setFillColor(sf::Color(0, 0, 0, 255));

	this->fontSize = fontSize;
	this->text.setCharacterSize(this->fontSize);
	this->setTextCenterPos(textCenterPos);

	this->currentColor = color;
	this->color = color;
	this->colorOnHover = colorOnHover;
	this->rectAmplifyingValue = rectAmplifyingValue;

}

bool Button::isHovering(sf::Vector2i mousePos) {

	sf::Vector2f updatedMousePos = sf::Vector2f(mousePos.x, mousePos.y);
	sf::RectangleShape rect = this->getRect();
	sf::Vector2f size = rect.getSize();
	sf::Vector2f pos = rect.getPosition();
	return (pos.x <= updatedMousePos.x  && updatedMousePos.x <= pos.x + size.x) && (pos.y <= updatedMousePos.y && updatedMousePos.y <= pos.y + size.y);
}



void Button::setTextCenterPos(sf::Vector2f newPos) {
	float textWidth = this->text.getLocalBounds().size.x;
	float textHeight = this->text.getLocalBounds().size.y;

	sf::Vector2f updatedPos = sf::Vector2f(newPos.x - textWidth / 2, newPos.y - textHeight / 2);
	this->text.setPosition(updatedPos);
}



void Button::draw(sf::RenderWindow& window) {
	
	sf::RectangleShape rect = this->getRect();
	rect.setFillColor(this->currentColor);
	window.draw(rect);
	window.draw(this->text);
}



bool Button::update(sf::RenderWindow& window,bool hasClicked) {
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (this->isHovering(mousePos)) {
		this->currentColor = this->colorOnHover;
		if (hasClicked) {
			return true;
		}
	}
	else{
		this->currentColor = this->color;
	}
	return false;

}

sf::RectangleShape  Button::getRect() {
	sf::Vector2f rectSize = this->text.getGlobalBounds().size;
	//Take into account bigger box for the button
	rectSize += sf::Vector2f(this->rectAmplifyingValue, this->rectAmplifyingValue);

	sf::RectangleShape rect(rectSize);
	rect.setPosition(this->getUpdatedRectPos());
	return rect;
}

//The position should be based on the top left corner of the text and scaled back according to the amplifying 
//scale 
sf::Vector2f Button::getUpdatedRectPos(){
	sf::Vector2f updatedRectPos(this->text.getGlobalBounds().position);

	sf::Vector2f textSize = this->text.getGlobalBounds().size;
	sf::Vector2f rectSize(textSize.x + this->rectAmplifyingValue, textSize.y + this->rectAmplifyingValue);
	
	updatedRectPos -= (rectSize - textSize) * 0.5f;
	
	return updatedRectPos;
}

Button::~Button() {
	
}