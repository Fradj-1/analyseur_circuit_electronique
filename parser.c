#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "circuit.h"
#include <assert.h>

/* ============================
*  déclaration fonctions privée
* ============================ */

// convertir les suffixe LTspice en nombre
static double convertion_suffix ( const char * str) ;

// convetir les noms de noeuds en chiffre
static int convertir_noeud(const char * token) ;

// détecter le type de composant
static t_type_composant detecter_type (char c) ;

/* ============================
*  déclaration fonctions publiques
* ============================ */

t_circuit *parser (const char * filename) ;

//  ***********************************
//  Definitions des fonctions privées
//  ***********************************


static double convertion_prefix ( const char * str)
{
    double value = atof( str );
    int taille = strlen ( str );
    char suffixe = str[taille - 1];

    switch ( suffixe )
    {
        case 'p': return value * 1e-12 ;
        case 'n': return value * 1e-9 ;
        case 'u': return value * 1e-6 ;
        case 'm': return value * 1e-3 ;
        case 'k': return value * 1e3 ;
        case 'g': return value * 1e9 ;
    }

    // pour le suffixe "meg"
    if (taille >= 3 && strcasecmp ( str + taille - 3, "meg") == 0 )
        return value * 1e6 ;

    return value ;

}

static int convertir_noeud(const char * token)
{
    // si'il y'a pas de noeud
    if (strcmp (token,"0")==0)
        return 0 ;
    // s'il y'a un noeud
    if (token[0] == 'N' || token[0] == 'n')
        return atoi (token+1) ;

    return atoi (token) ;
}

static t_type_composant detecter_type (char c)
{
    switch ( c )
    {
    case 'R' : return COMPOSANT_RESISTANCE ;
    case 'C' : return COMPOSANT_CONDENSATEUR ;
    case 'L' : return COMPOSANT_INDUCTEUR ;
    case 'V' : return COMPOSANT_VSOURCE ;
    case 'I' : return COMPOSANT_ISOURCE ;
    default : return COMPOSANT_RESISTANCE ;
    }
}

//  ***********************************
//  Definitions des fonctions publiques
//  ***********************************

t_circuit *parser (const char * filename)
{
    FILE *f = fopen ( filename, "r" ) ;
    if (!f)
    {
        perror("Erreur ouverture de fichier") ;
        return NULL ;
    }
    t_circuit *circuit = calloc(1,sizeof(t_circuit)) ;
    if (!circuit)
    {
        fclose(f) ;
        return NULL ;
    }
    char line[256] ;
    while (fgets(line,sizeof(line),f))
    {
        if (line[0] == '*' || line[0] == '\n' )
            continue ;

        char nom[16], n1s[16],n2s[16], vals[32] ;
        t_composant composant  = {0} ;

        int n = sscanf(line, "%15s %15s  %15s  %31s", nom, n1s, n2s, vals ) ;

        if ( n< 4)
            continue ;

        strncpy(composant.nom, nom, sizeof(composant.nom)) ;
        composant.type = detecter_type( nom[0] ) ;
        composant.noeud1 = convertir_noeud(n1s) ;
        composant.noeud2 = convertir_noeud(n2s) ;

        composant.valeur = convertion_suffix(vals) ;

        // detecter circuit AC
        char *ac = strstr(line, "AC") ;
        if (ac)
        {
            sscanf(ac, "AC %lf %lf",&composant.ac_amplitude,&composant.ac_phase) ;
        }

        // Ajout au circuit
        void *tmp = realloc(circuit->composants,(circuit->nombre_composant+1)*sizeof(t_composant)) ;
        if (!tmp)
        {
            fprintf(stderr, "Erreur realloc  composants\n") ;
            free(circuit->composants) ;
            free(circuit) ;
            fclose(f) ;
            return NULL ;
        }

        circuit ->composants = tmp;

        circuit->composants[circuit->nombre_composant++] = composant ;

        // Mise à jour du nombre de noeud
        if (composant.noeud1 > circuit->nombre_composant) circuit->nombre_composant = composant.noeud1 ;
        if (composant.noeud2 > circuit->nombre_composant) circuit->nombre_composant = composant.noeud2 ;
    }
    fclose(f) ;
    return circuit ;


}
