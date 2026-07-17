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
    COMPOSANT_INDUCTANCE,  // Inductance
} t_type_composant;

/* ============================
*  Caractéristique d'un composant
* ============================ */


typedef struct s_composant
{
    char nom [20] ;
    t_type_composant type ;
    int noeud_positif ;
    int noeud_negatif ;
    double valeur ;

    double ac_amplitude;
    double ac_phase ;
} t_composant;

typedef struct s_circuit
{
    int nombre_noeud ;
    int nombre_composant ;
    t_composant *composant ;
} t_circuit ;

#endif