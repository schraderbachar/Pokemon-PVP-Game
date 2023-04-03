

#ifndef FILE_PARSING_H
#define FILE_PARSING_H
class Pokemon
{
public:
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
};

class Moves
{
public:
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
};

class Pokemon_moves
{
public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
};

class Pokemon_species
{
public:
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
};

class Experience
{
public:
    int growth_rate_id;
    int level;
    int experience;
};

class Type_names
{
public:
    int type_id;
    int local_language_id;
    std::string name;
};

class Pokemon_stats
{
public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
};

class Stats
{
public:
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;
};

class Pokemon_types
{
public:
    int pokemon_id;
    int type_id;
    int slot;
};

class AllPokemonArrays
{
public:
    Pokemon *pokemonList;
    Pokemon_moves *pokemonMovesList;
    Pokemon_species *pokemonSpeciesList;
    Experience *experienceList;
    Type_names *typeNamesList;
    Pokemon_stats *pokemonStatsList;
    Stats *statsList;
    Moves *movesList;
    Pokemon_types *pokemonTypesList;
    int pokemonListLength;
    int pokemonMovesListLength;
    int pokemonSpeciesListLength;
    int experienceListLength;
    int typeNamesListLength;
    int pokemonStatsListLength;
    int movesListLength;
    int pokemonTypesListLength;
    int statsListLength;
};

void parsePokemonList(AllPokemonArrays *allArrays, std::string filename);
void parseMovesList(AllPokemonArrays *allArrays, std::string filename);
void parsePokemonMovesList(AllPokemonArrays *allArrays, std::string filename);
void parsePokemonSpeciesList(AllPokemonArrays *allArrays, std::string filename);
void parseExperienceList(AllPokemonArrays *allArrays, std::string filename);
void parseTypeNamesList(AllPokemonArrays *allArrays, std::string filename);
void parsePokemonStatsList(AllPokemonArrays *allArrays, std::string filename);
void parsePokemonTypesList(AllPokemonArrays *allArrays, std::string filename);
void parseStatsList(AllPokemonArrays *allArrays, std::string filename);

#endif