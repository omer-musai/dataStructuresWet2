#ifndef PLAYER_H
#define PLAYER_H


class Player
{
    private:
        int player_id;
        int group_id;
        int score;
        int level;

        public:
            Player(int player_id, int group_id, int score) : player_id(player_id), group_id(group_id), score(score), level(0) {}


            int getPlayerId()
            {
                return player_id;
            }
            int getGroupId()
            {
                return group_id;
            }
            int getScore()
            {
                return score;
            }
            int getLevel()
            {
                return level;
            }
            void setScore(int new_score)
            {
                this->score = new_score;
            }
            void setLevel(int new_level)
            {
                this->level = new_level;
            }

};


#endif //PLAYER_H