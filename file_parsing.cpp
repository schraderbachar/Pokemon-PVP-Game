#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include "file_parsing.h"

void parsePokemonList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].species_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].height = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].weight = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].base_experience = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].order = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->pokemonList[idx].is_default = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}

void parseMovesList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].generation_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].type_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].power = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].pp = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].accuracy = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].priority = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].target_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].damage_class_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].effect_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].effect_chance = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].contest_type_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].contest_effect_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->movesList[idx].super_contest_effect_id = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}
void parsePokemonMovesList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].version_group_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].move_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].pokemon_move_method_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].level = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonMovesList[idx].order = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}
void parsePokemonSpeciesList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].generation_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].evolves_from_species_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].evolution_chain_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].color_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].shape_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].habitat_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].gender_rate = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].capture_rate = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].base_happiness = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].is_baby = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].hatch_counter = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].has_gender_differences = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].growth_rate_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].forms_switchable = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].is_legendary = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].is_mythical = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].order = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonSpeciesList[idx].conquest_order = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}

void parsePokemonTypesList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonTypesList[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonTypesList[idx].type_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonTypesList[idx].slot = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}
void parseExperienceList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->experienceList[idx].growth_rate_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->experienceList[idx].level = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->experienceList[idx].experience = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}

void parsePokemonStatsList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonStatsList[idx].pokemon_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonStatsList[idx].stat_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonStatsList[idx].base_stat = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT_MAX";
            }
            allArrays->pokemonStatsList[idx].effort = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}

void parseTypeNamesList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            std::string tempTypeId = "INT MAX";
            if (sub.length() > 0)
            {
                tempTypeId = sub;
            }
            getline(strStream, sub, ',');
            if (sub == "9")
            {
                allArrays->typeNamesList[idx].type_id = std::stoi(tempTypeId);
                allArrays->typeNamesList[idx].local_language_id = 9;
                getline(strStream, sub, ',');
                if (sub.length() == 0)
                {
                    sub = "INT MAX";
                }
                allArrays->typeNamesList[idx].name = sub;
            }
            idx++;
        }
    }
    input.close();
}

void parseStatsList(AllPokemonArrays *allArrays, std::string filename)
{
    std::ifstream input;
    std::string line;
    int idx = 0;

    input.open(filename);
    std::getline(input, line);
    while (input)
    {
        std::getline(input, line);
        if (line.length() > 0)
        {
            std::stringstream strStream(line);
            std::string sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->statsList[idx].id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->statsList[idx].damage_class_id = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->statsList[idx].identifier = sub;
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->statsList[idx].is_battle_only = std::stoi(sub);
            getline(strStream, sub, ',');
            if (sub.length() == 0)
            {
                sub = "INT MAX";
            }
            allArrays->statsList[idx].game_index = std::stoi(sub);
            idx++;
        }
    }
    input.close();
}