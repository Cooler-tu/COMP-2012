#include <iostream>

#include "Card.hpp"
#include "CardPile.hpp"
#include "Entity.hpp"
#include "GameState.hpp"

/**
 * @brief Prints template message for when you deal damage.
 *
 * This function has been implemented for you. It is recommended for you to use
 * this helper function instead of calling `std::cout` directly in order to
 * reduce the chances of failing tests due to minor spelling mistakes.
 *
 * @param is_player_turn Whether it is the Player's turn or not.
 * @param damage The amount of damage the caster has dealt to the target after it was reduced by block.
 */
void print_deal_damage_message(bool is_player_turn, int damage) {
    if (is_player_turn) {
        std::cout << "  You dealt " << damage << " damage to the Enemy!" << std::endl;
    } else {
        std::cout << "  The Enemy dealt " << damage << " damage to you!" << std::endl;
    }
}

/**
 * @brief Prints template message for when you gain block.
 *
 * This function has been implemented for you. It is recommended for you to use
 * this helper function instead of calling `std::cout` directly in order to
 * reduce the chances of failing tests due to minor spelling mistakes.
 *
 * @param is_player_turn Whether it is the Player's turn or not.
 * @param block The amount of block the caster has gained.
 */
void print_gain_block_message(bool is_player_turn, int block) {
    if (is_player_turn) {
        std::cout << "  You gained " << block << " block!" << std::endl;
    } else {
        std::cout << "  The Enemy gained " << block << " block!" << std::endl;
    }
}

/**
 * @brief Draws card(s) from the Draw Pile to the Player's Hand.
 *
 * When this function is called, the Player draws card(s) from the Draw Pile to
 * the Player's Hand equal to the card's potency. It is assumed that only the
 * Player can play any cards with this effect, i.e., the Enemy will never hold
 * any cards with the card type `DRAW_CARDS`.
 *
 * This function has been implemented for you.
 *
 * @param game_state The current game state which you are meant to modify.
 */
void Card::draw_cards(GameState* game_state) const {
    std::cout << "  You drew " << potency << " card(s)!" << std::endl;
    for (int i = 0; i < potency; i++) {
        game_state->draw_card();
    }
}

// TODO: Complete your code down below!
CardPile::CardPile(Card** cards, int card_count, int max_card_count)
{
    this->card_count = card_count;
    this->max_card_count = max_card_count;
    this->cards = new Card*[max_card_count];
    for(int i = 0; i < card_count; i++)
        this->cards[i] = cards[i];
}

CardPile::~CardPile()
{
    delete[] cards;
}

void CardPile::add_card(Card* card)
{
    cards[card_count++] = card;
}

Card* CardPile::get_card(int index) const
{
    if(index >= card_count || index < 0) return nullptr;
    return cards[index];
}

Card* CardPile::remove_card(int index)
{
    if(index < 0 || index >= card_count) return nullptr;
    Card* r = get_card(index);
    for(int i = index+1; i < card_count; i++)
        cards[i-1] = cards[i];
    cards[card_count-1] = nullptr;
    card_count--;
    return r;
}

void CardPile::move_cards_to(CardPile& target_pile)
{
    for(int i = 0; i < this->card_count; i++)
    {
        target_pile.cards[i+target_pile.card_count] = this->cards[i];
        this->cards[i] = nullptr;
    }
    target_pile.card_count += this->card_count;
    this->card_count = 0;
}

void Card::deal_damage(GameState* game_state) const
{
    Entity* x = nullptr;
    int remain = 0;
    if(game_state->get_is_player_turn())
        x = game_state->get_enemy();
    else x = game_state->get_player();
    if(x->block >= potency) x->block -= potency;
    else
    {
        remain = potency - x->block;
        x->block = 0;
        x->health -= remain;
    }
    print_deal_damage_message(game_state->get_is_player_turn(), remain);
}

void Card::gain_block(GameState* game_state) const
{
    Entity* x = nullptr;
    if(!game_state->get_is_player_turn())
        x = game_state->get_enemy();
    else x = game_state->get_player();
    x->block += potency;
    print_gain_block_message(game_state->get_is_player_turn(), potency);
}