#include <iostream>
#include <deque>
#include <fstream>
#include <random>
#include <cstdlib>
#include <stdexcept>

#define M_FILE "data/monster.dat"
#define P_FILE "data/player.dat"
#define B_FILE "data/boss.dat"

#define CLEAR system("clear")
#define PRINTLN printf("\n")

struct Monster {
  std::string name;
  int HP; int MAX_HP;
  int ATK; int DEF;

  void print(){
    printf("%s HP:%d/%d ", name.c_str(), HP, MAX_HP);
    //printf("ATK:%d DEF:%d\n", ATK, DEF);
  }
  void println(){print();printf("\n");}

  void lvl_set(int lvl){
    HP *= lvl; MAX_HP *= lvl;
    ATK *= lvl; DEF *= lvl;
  }
  //struct Monster
};

class Player{
  std::string name; int LVL; int HP; int MAX_HP;
  int ATK; int DEF; int EXP; int STG; int TURN/*to Boss*/;
  std::deque<int> monster_stack;
public:
  int st_top(){return monster_stack.back();}
  void st_pop(){monster_stack.pop_back();}
  void st_push(int n){monster_stack.push_back(n);}
  int get_stg(){return STG;}

  void print(){
    printf("player %s LVL:%d HP:%d/%d\n", name.c_str(), LVL, HP, MAX_HP);
    printf("ATK:%d DEF:%d EXP:%d STG:%d TURN:%d\n", ATK, DEF, EXP, STG, TURN);
    if(monster_stack.size()>0){
      printf("Monster Stack : ");
      for(int x: monster_stack)printf("%d ", x);
      printf("\n");
    }
  }

  void turn_plus(){TURN++;}
  void next_stage(){STG++;TURN=0;}

  bool is_empty(){
    return monster_stack.empty();
  }

  bool get_data(std::string ifname){
    std::ifstream pf(ifname);
    if(pf){
      pf>>name>>LVL>>HP>>MAX_HP>>ATK>>DEF>>EXP>>STG>>TURN;
      int x;
      while(pf>>x)monster_stack.push_back(x);
      return true;
    }else{
      return false;
    }
  }

  bool set_data(std::string ofname){
    std::ofstream pf(ofname);
    if(pf){
      pf<<name<<" "<<LVL<<" "<<HP<<" "<<MAX_HP<<" "
      <<ATK<<" "<<DEF<<" "<<EXP<<" "<<STG<<" "<<TURN<<std::endl;
      for(int x: monster_stack)pf<<x<<" ";
      return true;
    }else{
      return false;
    }
  }

  bool lvl_check(){
    if(log2(EXP)+1>=LVL+1){
      int current = LVL;
      LVL = (int)log2(EXP)+1;
      double t = (double)(LVL)/current;
      MAX_HP *= t;
      HP = MAX_HP;
      ATK *= t;
      DEF *= t;
      //EXP -= log2(EXP)+1;
      std::cout<<"LVL UP! "<<current<<" -> "<<LVL<<std::endl;
      return true;
    }else{
      return false;
    }
  }

  void heal_full(){HP = MAX_HP;}

  /* return is_win */
  bool battle(Monster &M, int m_lvl){
    CLEAR;
    std::random_device seed_gen;
    std::mt19937 rnd(seed_gen());
    bool rtn = false;
    turn_plus();
    print();printf("\nvs. ");
    M.print();printf("LVL:%d\n", m_lvl);
    int getexp = std::max(1, (M.ATK+M.DEF)*M.MAX_HP/MAX_HP);
    std::string c; int p_atk, p_def, m_atk, m_def;
    PRINTLN;
    do{
      printf("1: attack 2: defence -> ");
      while(std::cin>>c){
        try{stoi(c);break;}
        catch(...){printf("input number 1 or 2");}
      }
    } while (c!="1"&&c!="2");
    switch(stoi(c)){
      case 1: p_atk = ATK+rnd()%(5*LVL); p_def = DEF;
      break; //武器使えるようにしたい
      case 2: p_atk = ATK; p_def = DEF+rnd()%(5*LVL);
      break;
    }

    int m_slct = rnd() % 2;
    if(m_slct)m_atk=M.ATK+rnd()%(5*m_lvl),m_def=M.DEF;
    else m_atk=M.ATK,m_def=M.DEF+rnd()%(5*m_lvl);
    
    M.HP -= std::max(1, (p_atk - m_def));
    HP -= std::max(1, (m_atk - p_def));

    if(M.HP>0 && HP>0){
      CLEAR;
      rtn = battle(M,m_lvl);
    }else if(M.HP<=0){
      printf("YOU WIN! get %d EXP!\n", getexp);
      EXP += getexp;
      lvl_check();
      print();
      return true;
    }else if(HP<=0){
      printf("YOU LOSE!\n");
      heal_full();
      return false;
    }
    return rtn;
  }
  //class Player
};

void ending();

int count_lines(std::string fname){
  int ans = 0;
  std::ifstream i(fname);
  if(i){
    std::string l;
    while(getline(i, l))ans++;
    i.close();
  }
  return ans;
}

Monster get_monster(int num){
  std::ifstream fin;
  Monster rtn;
  std::string s; int h,a,d,i=0;
  if(num>=0){
    //通常モンスター
    fin.open(M_FILE);
  }else{
    //ボス
    fin.open(B_FILE);
    num = ~num;//-1,-2,... -> 0,1,...
  }

  while(fin>>s>>h>>a>>d){
    if(i==num){
      rtn = {s, h, h, a, d};
      break;
    }
    i++;
  }
  fin.close();
  return rtn;
}

int main(){
  using namespace std;
  random_device seed_gen;
  mt19937 rnd(seed_gen());
  
  Player p;
  if(!p.get_data(P_FILE)){
    printf("p_file open error!\n");
    return -1;
  }

  ifstream m_file(M_FILE);
  if(!m_file){
    printf("m_file open error!\n");
    return -1;
  }
  m_file.close();
  int monster_all = count_lines(M_FILE);
  int boss_all = count_lines(B_FILE);

  while(!p.is_empty()){
    CLEAR;
    //int selected_monster = rnd()%monster_all; //cin>>selected_monster;
    //printf("enemy number 1-%d -> %d\n", monster_all, selected_monster);
    int encounter_monser[3];
    for(int em=0; em<3; em++){
      encounter_monser[em] = rnd()%monster_all;
    }
    p.print();
    printf("monster : push to Stack\n");
    for(int i=0;i<3;i++){
      printf("%d : ", i+1);
      get_monster(encounter_monser[i]).println();
    }
    printf("or battle with top of Stack\n");
    printf("0 : ");get_monster(p.st_top()).println();

    string slct;
    int selected_monster;
    while(cin>>slct){
      try {
        selected_monster = stoi(slct);
        break;
      } catch(...) {
        printf("input number 1-3 or 0\n");
      }
    }

    if(selected_monster == 0){
      //stack top
      selected_monster = p.st_top();
      p.st_pop();
    }else{
      //push to stack
      p.st_push(encounter_monser[selected_monster-1]);
      continue;
    }

    //バトル
    Monster enemy = get_monster(selected_monster);//stackのtop
    int e_lvl = p.get_stg() + rnd()%(int)log2(p.get_stg()+1);
    enemy.lvl_set(e_lvl);
    bool is_win = p.battle(enemy, e_lvl);
    
    if(p.is_empty()){
      if(is_win){
        if(selected_monster == ~boss_all+1){
          //全ボスクリア
          ending();
          break;
        } else {
          //次ボス装填
          p.next_stage();
          p.st_push(selected_monster - 1);
        }  
      } else {
        //ボス再チャレンジ
        p.st_push(selected_monster);
      }
    }
    if(p.set_data(P_FILE))printf("data saved!\n");

    printf("push [Q] to end. else to continue -> ");
    char c;cin>>c;
    if(c=='q')break;
  }

}

void ending(){
  printf("GAME CLEAR!\n");
}