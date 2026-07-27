#ifndef  circuit_h
#define  circuit_h

/* ============================
*  Types de base pour DC et AC
* ============================ */

 /* Nombre complexe pour l'analyse AC */
typedef struct s_complex
{
    double reelle ; // partie réelle
    double imaginaire ; // partie imaginaire
};

 /* Type de composant du circuit */
typedef  enum
{
    COMPOSANT_RESISTANCE, // résistance
    COMPOSANT_VSOURCE, // Source de tension
    COMPOSANT_ISOURCE, // Source de courant
    COMPOSANT_CONDENSATEUR, // Condensateur
    COMPOSANT_INDUCTEUR,  // Inductance
} t_type_composant;

/* ============================
*  Caractéristique d'un composant
* ============================ */


typedef struct s_composant
{
    char nom [20] ;  // nom du composant (R1,C2,V3, etc)
    t_type_composant type ; // type
    int noeud1 ; // numéro de noeuds ( 0 = masse)
    int noeud2 ;
    double valeur ; // valeur principale ( R en ohms, C en farade,L en henrys, V  en volts, I  en ampères)

    double ac_amplitude; // amplitude AC
    double ac_phase ; // phase AC en degrés
} t_composant;

typedef struct s_circuit
{
    int nombre_noeud ;   // nombre maximal de noeuds rencontrés (hors masse)
    int nombre_composant ;// nombre de composants
    t_composant *composants; // tableau dynamique de composants
} t_circuit ;

#endif