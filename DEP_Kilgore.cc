#include "Player.hh"
using namespace std;

#define PLAYER_NAME DEP_Kilgore

const int X[8] = { 1, 1, 0, -1, -1, -1,  0,  1 };
const int Y[8] = { 0, 1, 1,  1,  0, -1, -1, -1 };
const int HX[4] = { 1, 0, -1, 0 };
const int HY[4] = { 0, 1, 0, -1 };

struct PLAYER_NAME : public Player {

  static Player* factory () {
    return new PLAYER_NAME;
  }

  bool pos_ok(int x, int y) {

    return 0 <= x and x < MAX and 0 <= y and y < MAX;

  }

  bool can_attack(int equip, int id) {

    Posicio s = dades(id).pos;
    for (int i = 0; i < 8; ++i) {
      int sx = s.x + X[i];
      int sy = s.y + Y[i];
      if (pos_ok(sx,sy)) {
	int id2 = quin_soldat(sx,sy);
	if (id2 and dades(id2).equip != equip) {   
	  ordena_soldat(id,sx,sy);
	  return true;	
	}
      }
    }
    return false;
  }

  bool paraca_can_move(int x, int y) {

    if (que(x,y) == AIGUA or que(x,y) == MUNTANYA) return false;
    if (temps_foc(x,y) != 0) return false;
    if (quin_soldat(x,y) != 0) return false;
    return true;

  }

  bool s_can_move(int equip, int x, int y) {

    if (que(x,y) == AIGUA or que(x,y) == MUNTANYA) return false;
    if (temps_foc(x,y) != 0) return false;
    int id2 = quin_soldat(x,y);
    if (id2 and dades(id2).equip == equip) return false;
    return true;

  }

  bool heli_can_move(int x, int y) {

    if (que(x,y) == MUNTANYA or quin_heli(x,y) != 0) return false;
    return true;

  }

  bool h_can_move_up(int x, int y) {

    for (int i = -4; i <= -2; ++i) {
      for (int j = -4; j <= 4; ++j) {
	int xx = x + i;
	int yy = y + j;
	if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX) {	  
	  if (que(xx,yy) == MUNTANYA or quin_heli(xx,yy) != 0) return false;
	}
      }
    }
    return true;
  }

  bool h_can_move_down(int x, int y) {

    for (int i = 2; i <= 4; ++i) {
      for (int j = -4; j <= 4; ++j) {
	int xx = x + i;
	int yy = y + j;
	if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX) {	  
	  if (que(xx,yy) == MUNTANYA or quin_heli(xx,yy) != 0) return false;
	}
      }
    }
    return true;
  }

  bool h_can_move_right(int x, int y) {

    for (int i = -5; i <= 5; ++i) {
      for (int j = 2; j <= 3; ++j) {
	int xx = x + i;
	int yy = y + j;
	if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX) {	  
	  if (que(xx,yy) == MUNTANYA or quin_heli(xx,yy) != 0) return false;
	}
      }
    }
    return true;
  }

  bool h_can_move_left(int x, int y) {

    for (int i = -5; i <= 5; ++i) {
      for (int j = -3; j <= -2; ++j) {
	int xx = x + i;
	int yy = y + j;
	if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX) {  
	  if (que(xx,yy) == MUNTANYA or quin_heli(xx,yy) != 0) return false;
	}
      }
    }
    return true;
  }

  bool toma_napalm(int equip, int id) {

    Posicio h = dades(id).pos;
    int enemigos = 0;
    int aliados = 0;
    for (int i = -ABAST; i <= ABAST; ++i) {
      for (int j = -ABAST; j <= ABAST; ++j) {
	int xx = h.x + i;
	int yy = h.y + j;
	if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX) {
	  int id2 = quin_soldat(xx,yy);
	  if (id2 and dades(id2).equip != equip) ++enemigos;
	  else if (id2 and dades(id2).equip == equip) ++aliados;
	}
      }
    }
    return (enemigos >= 3 and enemigos > aliados);

  }

  void go_helicopter(int equip, int id, vector<Posicio> &vpos) {

    vector< vector<int> > W(60,vector<int>(60,-1));
    Posicio p = dades(id).pos;
    W[p.x][p.y] = 0;    
    queue<Posicio> Cola;    
    for (int k = 0; k < (int)vpos.size(); ++k) {    
      Cola.push(vpos[k]);     
      W[vpos[k].x][vpos[k].y] = 1;    
    }  
    bool found = false;
    while (not Cola.empty()) {
      Posicio c = Cola.front(); 
      Cola.pop();
      if (found) {
	Posicio aux;
	while (W[c.x][c.y] != 0) {  
	  for (int j = 0; j < 4; ++j) {	    
	    aux.x = c.x + HX[j];	    
	    aux.y = c.y + HY[j];	    
	    if (W[aux.x][aux.y] == (W[c.x][c.y])-1) c = aux;	    
	    if (W[c.x][c.y] == 0) {
	      int ori = dades(id).orientacio;  
	      if (j == 0) {
		if (ori == 2) ordena_helicopter(id,AVANCA1);
		else {
		  if (ori == 0) ordena_helicopter(id,RELLOTGE);
		  else if (ori == 1) ordena_helicopter(id,CONTRA_RELLOTGE);
		  else ordena_helicopter(id,RELLOTGE);
		}
	      }
	      else if (j == 1) {
		if (ori == 3) ordena_helicopter(id,AVANCA1);
		else {
		  if (ori == 1) ordena_helicopter(id,RELLOTGE);
		  else if (ori == 2) ordena_helicopter(id,CONTRA_RELLOTGE);
		  else ordena_helicopter(id,RELLOTGE);
		}
	      }
	      else if (j == 2) {
		if (ori == 0) ordena_helicopter(id,AVANCA1);
		else {
		  if (ori == 2) ordena_helicopter(id,RELLOTGE);
		  else if (ori == 1) ordena_helicopter(id,RELLOTGE);
		  else ordena_helicopter(id,CONTRA_RELLOTGE);
		}
	      }	      
	      else {
		if (ori == 1) ordena_helicopter(id,AVANCA1);
		else {
		  if (ori == 2) ordena_helicopter(id,RELLOTGE);
		  else if (ori == 3) ordena_helicopter(id,RELLOTGE);
		  else ordena_helicopter(id,CONTRA_RELLOTGE);
		}
	      }
	      j = 4;
	    }
	  }
	}
      }
      else {
	for (int i = 0; i < 4; ++i) {
	  int cx = c.x + HX[i];
	  int cy = c.y + HY[i];
	  if (pos_ok(cx,cy) and heli_can_move(cx,cy) and W[cx][cy] == -1) {
	    Posicio c2;
	    c2.x = cx;
	    c2.y = cy;
	    if (de_qui_post(cx,cy) != 0 and de_qui_post(cx,cy) != equip) {  
	      found = true;	     
	      i = 4;	      
	      while (not Cola.empty()) Cola.pop();    
	      Cola.push(c2);	      
	      W[c2.x][c2.y] = W[c.x][c.y]+1;	      
	    }   
	    else {	      
	      Cola.push(c2);	      
	      W[c2.x][c2.y] = W[c.x][c.y]+1;
	    }
	  }
	}
      }
    }
  }

  void go_soldiers(int equip, int id) {

    vector< vector<int> > W(60,vector<int>(60,-1));
    Posicio p = dades(id).pos;
    Posicio muevete;
    muevete.x = p.x;
    muevete.y = p.y;
    queue<Posicio> Cola;
    Cola.push(p);
    W[p.x][p.y] = 0;
    bool found = false;
    while (not Cola.empty()) {
      Posicio c = Cola.front();
      Cola.pop();
      if (found) {
	Posicio aux;
	while (W[c.x][c.y] != 0) {
	  for (int j = 0; j < 8; ++j) {
	    aux.x = c.x + X[j];
	    aux.y = c.y + Y[j];
	    if (W[aux.x][aux.y] == (W[c.x][c.y])-1) c = aux;
	    if (W[c.x][c.y] == 0) {
	      if (j == 0) {
		muevete.x += X[4];
		muevete.y += Y[4];
	      }
	      else if (j == 1) {
		muevete.x += X[5];
		muevete.y += + Y[5];
	      }
	      else if (j == 2) {
		muevete.x += X[6];
		muevete.y += Y[6];
	      }
	      else if (j == 3) {
		muevete.x += X[7];
		muevete.y += Y[7];
	      }
	      else if (j == 4) {
		muevete.x += X[0];
		muevete.y += Y[0];
	      }
	      else if (j == 5) {
		muevete.x += X[1];
		muevete.y += Y[1];
	      }
	      else if (j == 6) {
		muevete.x += X[2];
		muevete.y += Y[2];
	      }
	      else {
		muevete.x += X[3];
		muevete.y += Y[3];
	      }
	      j = 8;
	    }
	  }
	}
      }
      else {
	for (int i = 0; i < 8; ++i) {
	  int cx = c.x + X[i];
	  int cy = c.y + Y[i];
	  if (pos_ok(cx,cy) and s_can_move(equip,cx,cy) and W[cx][cy] == -1) {
	    Posicio c2;
	    c2.x = cx;
	    c2.y = cy;
	    if (de_qui_post(cx,cy) != 0 and de_qui_post(cx,cy) != equip) {
	      found = true;
	      i = 8;	    
	      while (not Cola.empty()) Cola.pop();	      
	      Cola.push(c2);	      
	      W[c2.x][c2.y] = W[c.x][c.y]+1;   
	    }	    
	    else {	      
	      Cola.push(c2);	      
	      W[c2.x][c2.y] = W[c.x][c.y]+1;
	    }
	  }
	}
      }
    }
    ordena_soldat(id, muevete.x, muevete.y);
  }

  virtual void play () {

    int equip = qui_soc();

    vector<int> vs = soldats(equip);

    vector<int> vh = helis(equip);

    for (int i = 0; i < (int)vs.size(); ++i) {

      if (not can_attack(equip,vs[i])) go_soldiers(equip,vs[i]);

    }

    bool napals = false;

    bool napals2 = false;

    int tirados = 0;

    for (int j = 0; j < (int)vh.size(); ++j) {

      Posicio p = dades(vh[j]).pos;

      if (dades(vh[j]).napalm == 0 and toma_napalm(equip,vh[j])) {

	if (j == 0) napals = true;

	else if (j == 1) napals2 = true;

	ordena_helicopter(vh[j],NAPALM);

      }

      else {

	vector<Posicio> vpos;

	for (int i = 0; i < 4; ++i) {

	  int px = p.x + HX[i];
	  
	  int py = p.y + HY[i];
	  
	  Posicio c;
	  
	  c.x = px;
	  
	  c.y = py;
	  
	  if (i == 0 and h_can_move_down(px,py)) vpos.push_back(c);
	  
	  else if (i == 1 and h_can_move_right(px,py)) vpos.push_back(c);
	  
	  else if (i == 2 and h_can_move_up(px,py)) vpos.push_back(c);
	  
	  else if (i == 3 and h_can_move_left(px,py)) vpos.push_back(c);
	  
	}
	
	go_helicopter(equip,vh[j],vpos);

      }

      if (j == 0 and not napals and (int)vs.size() < 60 and not dades(vh[j]).paraca.empty()) {

	int num_paracas = dades(vh[j]).paraca.size();

	for (int m = -ABAST; m <= ABAST; ++m) {

	  for (int n = -ABAST; n <= ABAST; ++n) {   

	    int xx = p.x + m;

	    int yy = p.y + n;

	    if (pos_ok(xx,yy) and paraca_can_move(xx,yy)) {

	      ordena_paracaigudista(xx,yy);

	      ++tirados;

	      --num_paracas;

	      if (num_paracas == 0 or tirados == 3) {

		m = ABAST + 1;
		  
		n = ABAST + 1;

	      }

	    }

	  }

	}
	
      }

      else if (j == 1 and not napals2 and (int)vs.size() < 60 and not dades(vh[j]).paraca.empty()) {

	int num_paracas = dades(vh[j]).paraca.size();

	for (int m = -ABAST; m <= ABAST; ++m) {

	  for (int n = -ABAST; n <= ABAST; ++n) {   

	    int xx = p.x + m;

	    int yy = p.y + n;

	    if (pos_ok(xx,yy) and paraca_can_move(xx,yy)) {

	      ordena_paracaigudista(xx,yy);

	      ++tirados;

	      --num_paracas;

	      if (num_paracas == 0 or tirados == 4) {

		m = ABAST + 1;
		  
		n = ABAST + 1;

	      }

	    }

	  }

	}
	
      }

    }

  }

};

/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);
