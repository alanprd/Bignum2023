#include "Bignum.hpp"

//PRADO Alan
//M1 CYBER
/* Temps chiffrement  + déchiffrement :     256 bits -> 130.32 s
                                            512 bits -> 1606.88 s 
                                            1024 bits ->   10355.09 s
                                            Tout est ok mais très lent 
*/

/*###################################################-CONSTRUCTEURS-#######################################################""#*/

Bignum::Bignum(int x){
    if(x >= 0){
        isPositive = true;
        tab.emplace_back(x);
    }else{
        isPositive = false;
        tab.emplace_back(-x);
    }
}

Bignum::Bignum(unsigned int x){
    isPositive = true;
    tab.emplace_back(x);
}


/**
 * @brief Réalise la division euclidienne par 2 pour convertir une chaîne en binaire.
 *
 * @param s La chaîne de caractères représentant un nombre décimal.
 * @return Une paire de chaînes de caractères, représentant le quotient et le reste de la division par 2.
 *
 * La méthode convertit une chaîne de caractères représentant un nombre décimal en
 * binaire en effectuant la division euclidienne par 2. Le résultat est retourné
 * sous forme d'une paire de chaînes de caractères, représentant le quotient et le reste.
 */
std::pair<std::string,std::string> divise2 (std::string const & s){ //permet de recup l'éccriture binaire
    std::string quotient="";
    int reste = 0;
    int q;
    for(int i=0; i < static_cast<int>(s.length()); i++){
        char digit = s[i];
        if (isdigit(digit)) {

            int num = digit - '0'; // Convert the character to an integer
            reste = reste * 10 + num;

            if(reste >= 2){
                q = reste/ 2;
                reste=reste % 2;
                quotient += std::to_string(q); 
            }else{
                quotient += std::to_string(0);            
            }
        }
    }

    std::pair <std::string, std::string> res(quotient, std::to_string(reste));// ajout du quotient reste dans la pair
    return res;
}


/**
 * @brief Constructeur pour initialiser un Bignum à partir d'une chaîne de caractères en base 10.
 *
 * @param s La chaîne de caractères représentant un nombre en base 10.
 *
 * Le constructeur initialise un objet Bignum à partir d'une chaîne de caractères
 * en base 10 en effectuant la conversion en binaire. Il divise répétitivement
 * la chaîne par 2 et récupère les restes pour obtenir la représentation binaire.
 * Ensuite, la représentation binaire est divisée en blocs de 32 bits, et chaque
 * bloc est converti en un entier non signé (uint32_t) qui est stocké dans le tableau.
 * Le signe du Bignum est déterminé par le premier caractère de la chaîne.
 */
Bignum::Bignum(std::string const & s){

    std::string num = s;//nb à diviser
    std::string bin;//chaine en binaire
    std::string q;//quotient
    char sign = num[0];
    if(sign == '+') isPositive = true;
    else if(sign == '-')isPositive = false;
    else isPositive = true;
    
    while(num.length() > 1 || num[0] >= 2){// sort de la boucle quand num contient 1  ou 0 donc non divisble par 2

        std::pair <std::string, std::string> res = divise2(num);

        q = res.first;  //division euclidienne         

        bin += res.second;//récupération du reste

        num=q;

        while(!num.empty() && num[0] == '0') {// pour enlever les zeros non significatif
            num.erase(0, 1);
        }
    }
     
    int j = 32;

    for(int k = 0; k <= static_cast<int>(bin.size()); k += 32){//parcours de la chaine binaire par saut de 32bits

        int p = 0;
        uint32_t n = 0;

        if(j >= static_cast<int>(bin.size())) j = bin.size();

        for(int i = k; i < j ; i++){//parcours des bits 
            n = n + ((bin[i] - '0') * pow(2, p));
            p++;
        }
        j += 32;
        tab.push_back(n);
    }   
}

Bignum::Bignum(const Bignum &num){
    tab = num.tab;
    isPositive = num.isPositive;

}
Bignum::Bignum(Bignum&& num){
    tab = std::move(num.tab);
    isPositive = std::move(num.isPositive);
}

Bignum::~Bignum(){ tab.clear(); }

/*##########################################################################################################################*/

/*######################################################-LIBRAIRY-##########################################################*/

/**
 * @brief Redimensionne le Bignum en supprimant les zéros non significatifs.
 *
 * La méthode parcourt le tableau du Bignum du poids fort au poids faible et
 * supprime les zéros non significatifs à la fin du tableau. Cela permet
 * d'ajuster la taille du tableau pour éliminer les zéros inutiles.
 */
void Bignum::resize(){
   
    int i = tab.size() - 1;
    while (i > 0 && tab[i] == 0) {// pour enlever les zeros non significatif
        tab.pop_back();
        i--;
    }
}

/*#### - fonctions du git utilisés car mes fonctions possédaient des cas limites amenant des erreurs de calcul - ###*/
Bignum addSameSign(Bignum const & x, Bignum const & y) {
  if (y.tab.size() > x.tab.size()) return addSameSign(y,x);
  Bignum z (0u);
  z.tab.resize(x.tab.size());
  uint64_t c = 0;
  for (unsigned i = 0; i < y.tab.size(); ++i) {
    uint64_t tmp = x.tab[i] + (y.tab[i] + c);
    z.tab[i] = tmp;
    c = tmp >> 32;
  }
  for (unsigned i = y.tab.size(); i < x.tab.size(); ++i) {
    uint64_t tmp = x.tab[i] + c;
    z.tab[i] = tmp;
    c = tmp >> 32;
  }
  if (c != 0) z.tab.emplace_back(1);
  return z;
}

Bignum SubtractX_Y(Bignum const & x, Bignum const & y) {
  // assume x >= y >= 0
  Bignum z (0u);
  z.tab.resize(x.tab.size());
  uint64_t c = 0;
  for (unsigned i = 0; i < y.tab.size(); ++i) {
    uint64_t tmp = x.tab[i] - (y.tab[i] + c);
    z.tab[i] = tmp;
    if ((tmp >> 32) != 0) c = 1;
    else c = 0;
  }
  for (unsigned i = y.tab.size(); i < x.tab.size(); ++i) {
    uint64_t tmp = x.tab[i] - c;
    z.tab[i] = tmp;
    if ((tmp >> 32) != 0) c = 1;
    else c = 0;
  }
  return z;
}

bool compareAbs(Bignum const & x, Bignum const & y) {
  unsigned nx = x.tab.size();
  while (nx > 0 && x.tab[nx-1] == 0) --nx;
  unsigned ny = y.tab.size();
  while (ny > 0 && y.tab[ny-1] == 0) --ny;
  if (nx != ny) return nx > ny;
  while (nx > 0) {
    --nx;
    if (x.tab[nx] != y.tab[nx]) return x.tab[nx] > y.tab[nx];
  }
  return true;
}

Bignum operator+(Bignum const & x, Bignum const & y) {
  if (x.isPositive == y.isPositive) {
    auto z = addSameSign(x,y);
    z.isPositive = x.isPositive;
    return z;
  }
  else {
    if (x.isPositive) {
      if (compareAbs(x,y)) return SubtractX_Y(x,y);
      auto z = SubtractX_Y(y,x);
      z.isPositive = false;
      return z;
    }
    else {
      if (compareAbs(y,x)) return SubtractX_Y(y,x);
      auto z = SubtractX_Y(x,y);
      z.isPositive = false;
      return z;
    }
  }
}

Bignum operator-(Bignum const & x, Bignum const & y) {
  if (x.isPositive == y.isPositive) {
    if (compareAbs(x,y)) {
      auto z = SubtractX_Y(x,y);
      z.isPositive = x.isPositive;
      return z;
    }
    else {
      auto z = SubtractX_Y(y,x);
      z.isPositive = !x.isPositive;
      return z;
    }
  }
  else {
    auto z = addSameSign(x,y);
    z.isPositive = x.isPositive;
    return z;
  }
}
/*### - fin fonctions du git - ###*/



/**
 * @brief Compare deux objets Bignum pour vérifier s'ils sont égaux.
 *
 * @param x Le premier Bignum.
 * @param y Le deuxième Bignum.
 * @return True si x est égal à y, sinon False.
 *
 * Compare les deux Bignum pour déterminer s'ils sont égaux. Les zéros non
 * significatifs sont supprimés avant la comparaison. La fonction prend en compte
 * le signe des Bignum pour la comparaison.
 */
bool operator==(Bignum const & x, Bignum const & y){

    Bignum a = x;
    a.resize();//on enleve les zero significatif si présent
    Bignum b = y;
    b.resize();

    int size1 = a.tab.size();
    int size2 = b.tab.size();

    if(a.isPositive && !b.isPositive)return false;// si x est + et que y est -
    if(!a.isPositive && b.isPositive) return false;//si x est - et que y est +
    if(size1>size2||size1<size2)return false;

    if((a.isPositive && b.isPositive)||(!a.isPositive && !b.isPositive)){//si les deux son + || les deux -
        for(int i=size1-1;i>=0;i--){//si ils ont la meme taille : on parcourt de gauche a droite le nombre et on compare la valeur
            if(a.tab[i]!=b.tab[i])return false;
        }
    }
    return true;
}

/**
 * @brief Compare deux objets Bignum pour vérifier si l'un est strictement supérieur à l'autre.
 *
 * @param x Le premier Bignum.
 * @param y Le deuxième Bignum.
 * @return True si x est strictement supérieur à y, sinon False.
 *
 * Compare les deux Bignum pour déterminer si x est strictement supérieur à y. Les
 * zéros non significatifs sont supprimés avant la comparaison. La fonction prend
 * en compte le signe des Bignum pour la comparaison.
 */
bool operator>(Bignum const & x, Bignum const & y){

    Bignum a = x;
    a.resize();//on enleve les zero significatif si présent
    Bignum b = y;
    b.resize();

    int size1 = a.tab.size();
    int size2 = b.tab.size();

    if(a.isPositive && !b.isPositive)return true;// si x est + et que y est -
    if(!a.isPositive && b.isPositive) return false;//si x est - et que y est +
    if(a.isPositive && b.isPositive){//si les deux son +
        if(size1>size2)return true;//si x est plus grand(en taille) que y
        if(size1<size2)return false;// si x est plus petit(en taille) que x
        for(int i=size1-1;i>=0;i--){//si ils ont la meme taille : on parcourt de gauche a droite le nombre et on compare la valeur
            if(a.tab[i]>b.tab[i])return true;//si le nombre à l'indice 1 de x est > que celui de y
            if(a.tab[i]< b.tab[i])return false;// <
        }
    }
    if(!a.isPositive && !b.isPositive){//on fait l'inverse pour deux valeurs negatives
        if(size1<size2)return true;
        if(size1>size2)return false;
        for(int i=size1-1;i>=0;i--){
            if(a.tab[i]<b.tab[i])return true;
            if(a.tab[i]> b.tab[i])return false;
        }
    }
    return false;
}
/**
 * @brief Opérateur de comparaison "inférieur à" pour les nombres Bignum.
 *
 * @param x Le premier nombre à comparer.
 * @param y Le deuxième nombre à comparer.
 * @return true si x est strictement inférieur à y, sinon false.
 */

bool operator<(Bignum const & x, Bignum const & y){ return (!(x>y) &&!(x==y));}

/**
 * @brief Opérateur de comparaison "inférieur ou égal à" pour les nombres Bignum.
 *
 * @param x Le premier nombre à comparer.
 * @param y Le deuxième nombre à comparer.
 * @return true si x est inférieur ou égal à y, sinon false.
 */
bool operator<=(Bignum const & x,Bignum const & y){ return (!(x>y));}

/**
 * @brief Opérateur de comparaison "supérieur ou égal à" pour les nombres Bignum.
 *
 * @param x Le premier nombre à comparer.
 * @param y Le deuxième nombre à comparer.
 * @return true si x est supérieur ou égal à y, sinon false.
 */
bool operator>=(Bignum const & x, Bignum const & y){ return ((x>y)||(x==y));}

/**
 * @brief Opérateur de comparaison "différent de" pour les nombres Bignum.
 *
 * @param x Le premier nombre à comparer.
 * @param y Le deuxième nombre à comparer.
 * @return true si x est différent de y, sinon false.
 */
bool operator!=(Bignum const & x,Bignum const & y){ return(!(x==y));}

/**
 * @brief Affiche la représentation hexadécimale du Bignum.
 *
 * Affiche la représentation hexadécimale du Bignum en préfixant le résultat par
 * "0x". Les chiffres hexadécimaux sont imprimés du poids fort au poids faible.
 */
void Bignum::printHexa()const{
    if (!isPositive) std::cout << "-";
    std::cout << "0x";
    for(int i=tab.size()-1;i>=0;i--) std::cout<<std::hex<<tab[i];
    std::cout<<""<<std::endl;
}

/*Bignum operator<<(Bignum const & num, unsigned int n){

    unsigned int nb_block = (n/32)+1;
    unsigned int bits= n%32;

    Bignum res = Bignum();
    res.isPositive= num.isPositive;
    res.tab.resize(num.tab.size()+nb_block);


    for(int i = num.tab.size()-1;i>=0;i--){
        uint32_t a = num.tab[i] & ((1u << (32-bits))-1);
        uint32_t b = num.tab[i] >> (32 - bits);
        int j = i + n/(32) + (bits> 0);
        res.tab[j]= res.tab[j]|b;
        res.tab[j-1]= (a << bits);
        
    }
    res.resize();
    return res;
}*/

Bignum & Bignum::operator<<=(unsigned int n){
    if (n == 0) return *this;
  unsigned n_bits = n%32;
  unsigned n_blocks = n/32;
  if (n_bits == 0) {
    tab.resize(tab.size() + n_blocks);
    for (unsigned i = tab.size(); i-- != n_blocks; ) tab[i] = tab[i-n_blocks];
  }
  else {
    tab.resize(tab.size() + n_blocks + 1);
    for (unsigned i = tab.size(); i-- != n_blocks+1; ) tab[i] = (tab[i-n_blocks] << n_bits) | (tab[i - (n_blocks + 1)] >> (32-n_bits));
    tab[n_blocks] = tab[0] << n_bits;
  }
  for (unsigned i = 0; i < n_blocks; ++i) tab[i] = 0u;

  unsigned ntab = tab.size();
  while (ntab > 1 && tab[ntab-1] == 0) --ntab;
  tab.resize(ntab);

  return *this;

}

Bignum & Bignum::operator>>=(unsigned n) {
  if (n == 0) return *this;
  unsigned n_bits = n%32;
  unsigned n_blocks = n/32;
  if (n_bits == 0) {
    for (unsigned i = 0; i + n_blocks < tab.size(); ++i) tab[i] = tab[i+n_blocks];
    if (tab.size() > n_blocks) tab.resize(tab.size() - n_blocks);
    else {
      tab.resize(1);
      tab[0] = 0;
    }
  }
  else {

    for (unsigned i = 0; i + 1 +  n_blocks < tab.size(); ++i) tab[i] = (tab[i+n_blocks] >> n_bits) | (tab[i + (n_blocks + 1)] << (32-n_bits));
    tab[tab.size() - n_blocks - 1] = tab[tab.size() - 1] >> n_bits;
  }

  unsigned ntab = tab.size();
  while (ntab > 1 && tab[ntab-1] == 0) --ntab;
  tab.resize(ntab);

  return *this;
}


Bignum &Bignum::operator=(Bignum &&num) {
  if (this != &num) {
    this->isPositive = num.isPositive;
    this->tab = move(num.tab);
  }
  return *this;
}
Bignum &Bignum::operator=(Bignum const &num) {
  if (this != &num) {
    this->isPositive = num.isPositive;
    this->tab = num.tab;
  }
  return *this;
}

/**
 * @brief Multiplie deux nombres Bignum.
 *
 * @param x Le premier facteur.
 * @param y Le deuxième facteur.
 * @return Le résultat de la multiplication.
 *
 * Cette fonction réalise la multiplication de deux nombres Bignum. Elle utilise
 * l'algorithme de la multiplication longue, avec des opérations arithmétiques sur
 * des entiers 32 bits. Le résultat est stocké dans un nouveau Bignum et renvoyé.
 */
Bignum operator*(Bignum const &x, Bignum const &y) {
    Bignum res;
    int n = x.tab.size();
    int t = y.tab.size();
    uint64_t c;
    uint64_t tmp;

    // Initialisation du tableau
    res.tab.resize(n + t);

    for (int i = 0; i < n; i++) {
        c = 0;
        for (int j = 0; j < t; j++) {
            tmp = (uint64_t)res.tab[i + j] + ((uint64_t)x.tab[i] * (uint64_t)y.tab[j]) + c;
            res.tab[i + j] = tmp % (1ULL<<32);
            c = tmp / (1ULL<<32);
        }
        res.tab[i + t] += c;
    }

    if(!x.isPositive ^ !y.isPositive)res.isPositive = false;// si un est negatif alors res est negatif
    res.resize();

    return res;
}


//problème resultat non attendu obtenu 
/*std::pair<Bignum, Bignum> division(Bignum x, Bignum const & y){

    if(y == Bignum(0))// si y = 0 , pas de division par 0
        return  std::make_pair(Bignum(0),Bignum(x));
    
    if(x < y)   // |x| doit etre sup a |y|
        return  std::make_pair(Bignum(0),x);
    
    if(x == y) return  std::make_pair(Bignum(1),Bignum(0));// a/a q=1 et r=0 ou -a/a = q= - 1 r=0

    unsigned t = y.tab.size()-1;//taille de y
    while (t >= 0 && y.tab[t] == 0) --t;
    unsigned n = 0;

    while ((y.tab[t] << n) < (1u << 31)) ++n;
    if (n != 0 || t == 0) {
        if (t == 0) n += 32;
        auto p = division(x << n, y << n);
        p.second >>= n;
        return p;
    }

    n = x.tab.size() - 1;
    while (n >= 0 && x.tab[n] == 0) --n;

    Bignum q = Bignum(0);//quotient
    Bignum r = Bignum(0);//reste


    q.tab.resize(n-t+1);//taile du bignum remplace la première boucle for du le pseudo-code

    while(x >= (y<<32*(n-t))){//x*b^i = x<<32i
        q.tab[n-t]+=1;
        x = x - (y<<32*(n-t));
    }
    for(unsigned i = n; i>=(t+1); --i ){
        if(x.tab[i] == y.tab[t])
            q.tab[i-t-1] = (1ULL<<32)- 1;
        else
            q.tab[i-t-1] = ((((uint64_t)x.tab[i] << 32) + x.tab[i-1])/y.tab[t]);
        
        Bignum a (y.tab[t]);
        a<<=32;
        a+= Bignum(y.tab[t-1]);
        Bignum b (x.tab[i]);
        b<<=32;
        b+= Bignum(x.tab[i-1]);
        b<<=32;
        b+=Bignum(x.tab[i-2]);

        while(Bignum(q.tab[i-t-1])* a > b){
            q.tab[i-t-1] -=1; 
        }
        x -= (Bignum(q.tab[i-t-1]) * y)  << (32 * (i-t-1));
        if(x < 0){
            x += (y << (32 * (i-t-1)));
            q.tab[i-t-1]-=1;
        }
    }
    q.resize();
    r.resize();
    return std::make_pair(q,x);
    
}
*/

/**
 * @brief Effectue la division de deux grands entiers.
 *
 * @param x Le dividende (numérateur).
 * @param y Le diviseur (dénominateur).
 * @return Une paire contenant le quotient et le reste de la division (x / y).
 *
 * Cette fonction divise le grand entier x par le grand entier y en utilisant
 * l'algorithme classique de division. Le résultat est retourné sous la forme d'une
 * paire, où le premier élément est le quotient et le deuxième élément est le reste.
 *
 * L'algorithme utilise une technique de normalisation du diviseur pour s'assurer
 * que le diviseur est suffisamment grand avant d'effectuer la division.
 */
std::pair<Bignum, Bignum> division(Bignum x, Bignum const & y){

    if(x < y) return std::make_pair(Bignum(0), x);

    if(x == y) return std::make_pair(Bignum(1), Bignum(0));

    //normalisation du diviseur
    auto Y = y;
     while(Y < x) Y <<= 1; //Augmente le diviseur Y en le décalant vers la gauche jusqu'à ce qu'il soit plus grand que x.
  
    if(Y > x) Y >>= 1;// on le ramène a la moitier pour qu'il soit inferieur a x

    auto quotient = Bignum(0);
    auto reste = x;

    while (Y >= y) {
        quotient <<= 1;
        // Si le reste est plus grand ou égal à Y, ajuste le quotient et le reste.
        if(reste >= Y) {
            quotient += 1;
            reste -= Y;
        }
        // Décale Y vers la droite pour la prochaine itération.
        Y >>= 1;
    }
    return std::make_pair(quotient, reste); 
}


/**
 * @brief Surcharge de l'opérateur de division pour les objets Bignum.
 *
 * @param x Le dividende.
 * @param y Le diviseur.
 * @return Le quotient de la division x / y.
 *
 * Utilise la fonction division() pour effectuer la division de x par y. Ajuste
 * le signe du quotient en fonction des signes de x et y. 
 */
Bignum operator/(Bignum const & x, Bignum const & y){
    auto a = x;
    auto b = y;
    a.isPositive = true;
    b.isPositive = true;
    auto q = division(std::move(a),std::move(b));
    if(!x.isPositive ^ !y.isPositive)
        q.first.isPositive = false; // si un des nombres est négatif alors le quotient sera negatif 
    return q.first;
}


/**
 * @brief Surcharge de l'opérateur de modulo pour les objets Bignum.
 *
 * @param x Le dividende.
 * @param y Le diviseur.
 * @return Le reste de la division x % y.
 *
 * Utilise la fonction division() pour obtenir le quotient et le reste de x par y.
 * Ajuste le signe du reste en fonction du signe de x.
 */ 
Bignum operator%(Bignum const & x, Bignum const & y){
    auto a = x;
    auto b = y;
    a.isPositive = true;
    b.isPositive = true;
    auto r = division(std::move(a),std::move(b));
    if (!x.isPositive) {
        r.second.isPositive = false;
        if (y.isPositive) r.second += y;
        else r.second -= y;
    }
    return r.second;
}

/**
 * @brief Opérateur d'assignation et d'addition composée pour les nombres Bignum.
 *
 * @param num Le nombre Bignum à ajouter.
 * @return Une référence vers l'objet Bignum modifié.
 */
Bignum &Bignum::operator+=(Bignum const & num){
    Bignum tmp = *this;
    *this = tmp + num;
    return *this;
}

/**
 * @brief Opérateur d'assignation et de soustraction composée pour les nombres Bignum.
 *
 * @param num Le nombre Bignum à soustraire.
 * @return Une référence vers l'objet Bignum modifié.
 */
Bignum &Bignum::operator-=(Bignum const & num){
    Bignum tmp = *this;
    *this = tmp - num;
    return *this;
}

/**
 * @brief Opérateur d'assignation et de multiplication composée pour les nombres Bignum.
 *
 * @param num Le nombre Bignum à multiplier.
 * @return Une référence vers l'objet Bignum modifié.
 */
Bignum &Bignum::operator*=(Bignum const & num){
    Bignum tmp = *this;
    *this = tmp * num;
    return *this;
}

/**
 * @brief Opérateur d'assignation et de division composée pour les nombres Bignum.
 *
 * @param num Le nombre Bignum à diviser.
 * @return Une référence vers l'objet Bignum modifié.
 */
Bignum &Bignum::operator/=(Bignum const & num){
    Bignum tmp = *this;
    *this = tmp / num;
    return *this;
}

/**
 * @brief Surcharge de l'opérateur de sortie pour les objets Bignum.
 *
 * @param flux Le flux de sortie.
 * @param x L'objet Bignum à afficher.
 * @return Le flux de sortie après affichage.
 *
 * Affiche l'objet Bignum sur le flux de sortie, en gérant le cas où x est égal à zéro.
 * Le signe est affiché en fonction de la positivité ou négativité de x.
 */
std::ostream & operator<<(std::ostream & flux, Bignum const & x) {
  // Si x est égal à zéro, afficher simplement "0".
  if (x == Bignum(0)) {
    flux << 0;
  } else {
    // Si x n'est pas positif, afficher le signe négatif.
    if (!x.isPositive) {
      flux << "-";
    }

    // Diviser x par 10 et obtenir le quotient (p.first) et le reste (p.second).
    auto p = division(x, 10);
    p.first.isPositive = true;

    // Si le quotient n'est pas égal à zéro, l'afficher.
    if (p.first != Bignum(0)) {
      flux << p.first;
    }

    // Afficher le chiffre des unités du reste.
    flux << p.second.tab[0];
  }
  return flux;
}

/*############################################################################################################*/

/*################################################-RSA-#######################################################*/


/**
 * @brief Exponentiation modulaire pour les objets Bignum.
 *
 * @param b La base.
 * @param e L'exposant.
 * @param modulo Le modulo.
 * @return Le résultat de (b^e) % modulo.
 *
 * Calcule l'exponentiation modulaire (b^e) % modulo de manière efficace en utilisant
 * l'algorithme de l'exponentiation rapide. Réduit modulo à chaque étape pour éviter
 * les valeurs trop grandes.
 * */
Bignum modular_exponentiation(Bignum const & b, Bignum const & e, Bignum const & modulo){
        
    if(e == Bignum(0)){ return Bignum(1); }

    Bignum res = Bignum(1);
    Bignum base = b % modulo;
    Bignum exposant = e;

    while(exposant != Bignum(0) ){

        //Si l'exposant est impair, on multiplie le résultat par la base et on reduit modulo m
        if(exposant.tab[0] % 2 != 0) res = (res * base) % modulo;
        exposant >>= 1;
        base = (base * base) % modulo;//élévation de la base au carré puis réduction avec modulo pour pas avoir de valeur trop grandes 
    }
    return res;
}

/**
 * @brief Algorithme d'Euclide étendu pour les objets Bignum.
 *
 * @param a Le premier entier.
 * @param b Le deuxième entier.
 * @return Une paire (x, y) telle que ax + by = gcd(a, b).
 *
 * Implémente l'algorithme d'Euclide étendu pour trouver les coefficients x et y tels
 * que ax + by = gcd(a, b).
 * 
 * Version implémenté en ADA
 */
std::pair<Bignum,Bignum> EEA(Bignum const & a, Bignum const & b){
    
    auto x0 = Bignum(1);
    auto x1 = Bignum(0);
    auto a0 = a;
    auto y0 = Bignum(0);
    auto y1 = Bignum(1);
    auto b0 = b;

    Bignum q,r,tmp;

    while (b0 > Bignum(0)){
        q = a0 / b0;
        r = a0 % b0;

        // Mises à jour des valeurs pour la prochaine itération.
        a0 = b0;
        b0 = r; 

        // Mises à jour des coefficients x.
        tmp = x1;
        x1 = x0 - (q * x1);
        x0 = tmp;

        // Mises à jour des coefficients y.
        tmp = y1;
        y1 = y0 - (q * y1);
        y0 = tmp;
    }
    // Correction du coefficient x si négatif.
    if(x0 < Bignum(0)){
        x0 +=  b;
    }
    return std::make_pair(x0,a0);
}

/**
 * @brief Calcule l'inverse modulaire d'un nombre Bignum par rapport à un autre.
 *
 * @param x Le nombre Bignum pour lequel on souhaite calculer l'inverse modulaire.
 * @param y L'autre nombre Bignum utilisé pour le calcul.
 * @return L'inverse modulaire de x par rapport à y.
 */
Bignum inverseMod(Bignum const & x, Bignum const & y){
    return EEA(x,y).first;
}

/**
 * @brief Calcule le PGCD de deux nombres Bignum.
 *
 * @param x Le premier nombre Bignum.
 * @param y Le deuxième nombre Bignum.
 * @return Le PGCD de x et y.
 */
Bignum pgcd(Bignum const & x, Bignum const & y){
    return EEA(x,y).second;
}

/**
 * @brief Génère un objet Bignum aléatoire.
 *
 * @param n Le nombre de bits souhaité pour le Bignum.
 * @return Un Bignum aléatoire de n bits.
 *
 * Génère un Bignum aléatoire en utilisant une distribution uniforme pour chaque
 * bloc de 32 bits. Le Bignum généré a une taille appropriée pour contenir n bits.
 */
Bignum bignum_gen(int n){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    Bignum result;
    result.isPositive = true;
    unsigned int numBlocks = n / 32;  // Calcul du nombre de blocs de 32 bits nécessaire
    unsigned bits = n % 32;     // bits restant

    if(n > 0) result.tab.resize(numBlocks + (bits != 0)); // on set la size du tableau avec le nombre de bloc de 32 bits puis et le reste si existant
    else result.tab.resize(1);
   
   // Génération de nombres aléatoires pour chaque bloc
    for (unsigned i = 0; i < numBlocks; i++) {
        result.tab[i] = dist(gen);
    }

    if(bits > 0) result.tab[numBlocks] = dist(gen) & (UINT32_MAX >> (32 - n % 32)); // on complete le dernier blocs si il y a un reste de bits dans le découpage précédent 

    return result;
}


/**
 * @brief Teste si un nombre est probablement premier en utilisant le petit théorème de Fermat.
 *
 * @param x Le nombre à tester.
 * @return True si x est probablement premier, sinon False.
 *
 * Utilise le petit théorème de Fermat pour tester si x est probablement premier.
 * Applique des tests pour les bases 2, 3, 5, et 7. Renvoie True si x passe tous les tests.
 */
bool is_prime(Bignum const & x){

    Bignum un = Bignum(1);
    Bignum deux = Bignum(2);
    Bignum trois = Bignum(3);
    Bignum cinq = Bignum(5);
    Bignum sept = Bignum(7);

    if( x <= un) return false; // 0 et 1 non premier

    // Test 2^(x-1) ≡ 1 (mod x)
    if (modular_exponentiation(deux, x - un, x) != un) return false;

    // Test 3^(x-1) ≡ 1 (mod x)
    if (modular_exponentiation(trois, x - un, x) != un) return false;

    
    // Test 5^(x-1) ≡ 1 (mod x)
    if (modular_exponentiation(cinq, x - un, x) != un) return false;

    // Test 7^(x-1) ≡ 1 (mod x)
    return modular_exponentiation(sept, x - un, x) == un;
}

/**
 * @brief Génère un nombre premier aléatoire de n bits.
 *
 * @param n Le nombre de bits souhaité pour le nombre premier.
 * @return Un nombre premier aléatoire de n bits.
 *
 * Génère un nombre premier aléatoire en utilisant la fonction bignum_gen() et en
 * appliquant le test de primalité avec le petit théorème de Fermat. Si le nombre
 * généré n'est pas premier, il est incrémenté par 2 jusqu'à ce qu'un nombre premier
 * soit trouvé.
 */
Bignum prime_gen(int n){
    Bignum probablePrime;
    bool isPrime = false;
    probablePrime = bignum_gen(n);

    if(probablePrime % Bignum(2) == Bignum(0)) probablePrime += Bignum(1); // si il est paire on le fait devenir impaire
    
    std::cout << "Generating";

    while (!isPrime) {
        std::cout << " .";
        std::cout.flush();
        if (is_prime(probablePrime) == 1) isPrime = true;
        else probablePrime += Bignum(2);// on ajoute 2 pour avoir le nombre impaire suivant
    }
    std::cout << std::endl;

    return probablePrime;
}

/**
 * @brief Génère une paire de clés RSA publiques et privées.
 *
 * @param numbits Le nombre de bits souhaité pour les nombres premiers p et q.
 * @return Une paire de clés RSA, composée d'une clé publique et d'une clé privée.
 *
 * Génère deux nombres premiers p et q de taille numbits, puis calcule la clé
 * publique (n, e) et la clé privée (n, d) en utilisant l'algorithme RSA.
 * La valeur fixe de e est utilisée (65537).
 */
std::pair<std::pair<Bignum, Bignum>, std::pair<Bignum, Bignum>> gen_key(int numbits){

    //génération des nombres premiers
    Bignum p = prime_gen(numbits);
    Bignum q = prime_gen(numbits);

    while(p == q){q = prime_gen(numbits);}// il faut que  p et q soit différents 
    
    // on prends e fixe
    Bignum e = Bignum("65537");//(2¹⁶) + 1

    auto phi =(p - Bignum(1)) * (q - Bignum(1));// Thm de Euleur

    /*std::cout << "p :" << p << std::endl;
    std::cout << "q :" << q << std::endl;
    std::cout << "phi :" << phi << std::endl;*/

    auto d = inverseMod(e, phi); // calcul de l'inverse de e modulo phi

    Bignum n = p * q;

    std::pair<Bignum,Bignum> public_key = std::make_pair(n, e);
    std::pair<Bignum,Bignum> private_key = std::make_pair(n, d);

    return std::make_pair(public_key,private_key);
}

/**
 * @brief Encode une chaîne de caractères en utilisant des entiers Bignum de n bits.
 *
 * @param msg La chaîne de caractères à encoder.
 * @param n Le nombre de bits pour chaque entier Bignum.
 * @return Un vecteur d'entiers Bignum résultants de l'encodage.
 *
 * Cette fonction prend en entrée une chaîne de caractères et encode chaque groupe
 * de n bits en utilisant des entiers Bignum. Les caractères de la chaîne sont ajoutés
 * à un accumulateur en le décalant de 8 positions vers la gauche et en ajoutant le
 * caractère. Lorsque le nombre d'octets ajoutés atteint n / 8, l'accumulateur est
 * ajouté au vecteur résultant, et l'accumulateur est réinitialisé. Le vecteur résultant
 * contient les entiers Bignum résultants de l'encodage.
 */
 std::vector<Bignum> encode(std::string msg, int  n){
    Bignum accumulator(0); // Initialiser un accumulateur à zéro.
    std::vector<Bignum> res; // Vecteur pour stocker les entiers résultants.
    int byteCount = 0; // Compteur du nombre d'octets ajoutés à l'accumulateur.

    // Parcourir chaque caractère de la chaîne de caractères.
    for(unsigned i = 0; i < msg.length(); i++){
        // Décaler les bits de l'accumulateur de 8 positions vers la gauche.
        accumulator <<= 8;
        // Ajouter le caractère actuel à l'accumulateur.
        accumulator += msg[i];
        // Incrémenter le compteur d'octets ajoutés à l'accumulateur.
        byteCount++;

        // Si le nombre d'octets ajoutés atteint n / 8 (car chaque octet a 8 bits).
        if(byteCount == n / 8){
            // Ajouter l'accumulateur au vecteur résultant.
            res.push_back(accumulator);
            // Réinitialiser l'accumulateur à zéro pour le prochain groupe d'octets.
            accumulator = 0;
            // Réinitialiser le compteur d'octets.
            byteCount = 0;
        }
    }

    // Vérifier s'il reste des octets non traités dans l'accumulateur.
    if (accumulator != 0) {
        // Ajouter l'accumulateur au vecteur résultant.
        res.push_back(accumulator);
    }

    return res;
}

/**
 * @brief Décode des entiers Bignum en une chaîne de caractères.
 *
 * @param encoded_msg Vecteur d'entiers Bignum à décoder.
 * @param n Le nombre de bits pour chaque entier Bignum.
 * @return La chaîne de caractères résultante du décodage.
 *
 * Cette fonction prend en entrée un vecteur d'entiers Bignum encodés avec la fonction
 * "encode". Elle extrait chaque octet de l'entier, le convertit en caractère, et l'ajoute
 * à la chaîne résultante. La fonction décale ensuite les bits de l'entier vers la droite
 * de 8 positions pour se préparer au traitement du prochain octet. Ce processus est répété
 * jusqu'à ce que le premier entier dans le vecteur soit égal à zéro. 
 * La chaîne résultante est renvoyée.
 */
std::string decode(std::vector<Bignum> encoded_msg, int n) {
    std::string res; 
    // Tant que le premier entier dans le vecteur est supérieur à zéro.
    while(encoded_msg[0] > 0) {
        // Si le dernier entier dans le vecteur est égal à zéro, le supprimer.
        if(encoded_msg[encoded_msg.size() - 1] == 0){
            encoded_msg.pop_back();
        }

        // Extraire le dernier octet de l'entier.
        Bignum byte = encoded_msg[encoded_msg.size() - 1] % Bignum(256);
        // Ajouter le caractère correspondant à l'octet à la chaîne résultante.
        res = static_cast<char>(byte.tab[0]) + res;

        // Décaler les bits de l'entier vers la droite de 8 positions pour se préparer au prochain octet.
        encoded_msg[encoded_msg.size() - 1] = encoded_msg[encoded_msg.size() - 1] >> 8;
    }

    return res;
}

Bignum encryptNb(Bignum x,std::pair<Bignum,Bignum> public_key){
    return modular_exponentiation(x,public_key.second,public_key.first);
}
Bignum decryptNb(Bignum x,std::pair<Bignum,Bignum> private_key){
    return modular_exponentiation(x,private_key.second,private_key.first);
}

/**
 * @brief Chiffre un vecteur de Bignum en utilisant la clé publique RSA.
 *
 * @param msg Le vecteur de Bignum à chiffrer.
 * @param public_key La clé publique RSA (n, e).
 * @return Le vecteur chiffré résultant.
 *
 * Chiffre chaque élément du vecteur `msg` en utilisant la clé publique RSA (n, e)
 * avec l'algorithme RSA. Le résultat chiffré est stocké dans un nouveau vecteur.
 */
std::vector<Bignum> encrypt(std::vector<Bignum> msg, std::pair<Bignum,Bignum> public_key){
    std::vector<Bignum> res;
    Bignum tmp;
    for(int i = 0; i < static_cast<int>(msg.size()); i++){
        tmp = modular_exponentiation(msg[i],public_key.second,public_key.first);
        res.push_back(tmp);
    }
    return res;
}

/**
 * @brief Déchiffre un vecteur de Bignum en utilisant la clé privée RSA.
 *
 * @param cipher Le vecteur de Bignum à déchiffrer.
 * @param private_key La clé privée RSA (n, d).
 * @return Le vecteur déchiffré résultant.
 *
 * Déchiffre chaque élément du vecteur `cipher` en utilisant la clé privée RSA (n, d)
 * avec l'algorithme RSA. Le résultat déchiffré est stocké dans un nouveau vecteur.
 */
std::vector<Bignum> decrypt(std::vector<Bignum> cipher, std::pair<Bignum,Bignum> private_key){
    std::vector<Bignum> res;
    Bignum tmp = Bignum (0);
    for(int i = 0; i < static_cast<int>(cipher.size()); i++){
        tmp = modular_exponentiation(cipher[i],private_key.second,private_key.first);
        res.push_back(tmp);
    }
    return res;
}

void print_vector(std::vector<Bignum> x){
     for (const auto& element : x) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

/*################################################-fin-RSA-#######################################################*/



int main() {
    /*Bignum a = Bignum("100000000000000");
    Bignum b = Bignum("6");
    auto res = a+b;
    auto res2 = a*b;
    auto res3 = a-b;
    auto res4 = a/b;
    auto res5=a%b;
    std::cout<<"100000000000000 + 6 = " << res << std::endl;
    std::cout << "100000000000000 * 6 = " << res2 << std::endl;
    std::cout <<"100000000000000 - 6 = " << res3 << std::endl;
    std::cout <<"100000000000000 / 6 = "<< res4 << ", 100000000000000 % 6 = " << res5 << std::endl;

 // Test de l'exponentiation modulaire
    Bignum base = 7;
    Bignum exposant = 3;
    Bignum modulo = 11;

    Bignum resultat_exp_mod = modular_exponentiation(base, exposant, modulo);
    std::cout << "Résultat de l'exponentiation modulaire : " << resultat_exp_mod << std::endl;
    // Test du calcul de l'inverse modulaire
    Bignum x = 3;
    Bignum y = 11;

    Bignum inverse_mod = inverseMod(x, y);
    std::cout << "Inverse modulaire de " << x << " modulo " << y << " : " << inverse_mod << std::endl;
    // Test du calcul du PGCD
    Bignum num1 = 48;
    Bignum num2 = 18;

    Bignum pgcd_result = pgcd(num1, num2);
    std::cout << "PGCD de " << num1 << " et " << num2 << " : " << pgcd_result << std::endl;

    Bignum gen = bignum_gen(256);
    std::cout << "Bignumgen : " << gen << std::endl;

    Bignum test = Bignum("3167");

    std::cout << "isPrime? : " << is_prime(test) << std::endl;

    std::cout << "encode : " << encode("Salut") << std::endl;*/


    std::string taille_str;
    int size;

    std::cout << "Entrez la taille (en bits) de la clé : ";
    std::getline(std::cin, taille_str);
    size = std::stoi(taille_str);

    auto keys = gen_key(size);
    auto pblc = keys.first;
    auto prvt = keys.second;
    std::string msg;

    std::cout << "Entrez le message que vous souhaitez chiffrer : ";
    std::getline(std::cin, msg);

    std::cout << "###-Chiffrement du message...-###" << std::endl;

    std::vector<Bignum> msg_encoded = encode(msg, size);
    std::cout << "Message encodé : " ; 
    print_vector(msg_encoded);

    std::vector<Bignum> cipher = encrypt(msg_encoded, pblc);
    std::cout << "Message chiffré : "; 
    print_vector(cipher);


    std::cout << "###-Déchiffrement du message...-###" << std::endl;

    std::vector<Bignum> clear_text = decrypt(cipher, prvt);
    std::cout << "Message déchiffré : ";
    print_vector(clear_text);

    std::string texte_decode = decode(clear_text, size);

    std::cout << "Message décodé : " << texte_decode << std::endl;

    return 0;

}