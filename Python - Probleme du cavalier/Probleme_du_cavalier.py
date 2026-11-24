import matplotlib.pyplot as plt

mouvements = [
    (2, 1),
    (2, -1),
    (-2, 1),
    (-2, -1),
    (1, 2),
    (1, -2),
    (-1, 2),
    (-1, -2)
]

#vérifie si la case est un sommet valide
def estValide(x, y, taille, lettres, plateau) -> bool:
    #vérifie si les coordonnées sont dans les limites du plateau
    if (x < 0 or x >= taille or y < 0 or y >= taille):
        return False
    #convertit les coordonnées en clé du dictionnaire
    case = lettres[x] + str(y + 1) #passage de int a str
    #retourne True si la case n'a pas encore été visitée
    return plateau[case] == 0

#vérifie si deux cases sont reliées par un déplacement
def estVoisin(x1, y1, x2, y2) -> bool:
    dx = abs(x1 - x2) #valeur absolue
    dy = abs(y1 - y2) #valeur absolue
    return (dx == 2 and dy == 1) or (dx == 1 and dy == 2) #si c'est un mouvement légal

#compte le nombre de voisins accessibles depuis une case
def compterVoisins(x, y, taille, lettres, plateau) -> int:
    compteur = 0
    #teste les 8 mouvements possibles en "L" du cavalier
    for dx, dy in mouvements:
        nx, ny = x + dx, y + dy #calcul des coordonnées de la destination
        #si la destination est sur le plateau et n'a pas été visitée
        if estValide(nx, ny, taille, lettres, plateau):
            compteur += 1 
    return compteur

def backtracking(x, y, compteur, taille, lettres, plateau, chemin_max=None) -> bool:
    #marque la case actuelle avec son numéro de passage
    case = lettres[x] + str(y + 1)
    plateau[case] = compteur

    #si un chemin max est demandé, on enregistre si on bat le record de longueur
    if chemin_max is not None and compteur > chemin_max[0]:
        chemin_max[0] = compteur
        chemin_max[1] = plateau.copy()

    #toutes les cases ont été visitées
    if compteur == taille * taille:
        return True

    #récupère les voisins valides et les trie par nombre de voisins
    voisins = []
    for dx, dy in mouvements:
        nx, ny = x + dx, y + dy
        if estValide(nx, ny, taille, lettres, plateau):
            voisins.append((compterVoisins(nx, ny, taille, lettres, plateau), nx, ny))
    voisins.sort()

    #teste récursivement chaque déplacement
    for nbVoisins, nx, ny in voisins:
        if backtracking(nx, ny, compteur + 1, taille, lettres, plateau, chemin_max):
            return True #chemin entier trouvé

    #backtrack, remet la case à 0 si le chemin mène à une impasse
    plateau[case] = 0
    return False

#fonction récursive de backtracking pour trouver un cycle hamiltonien
def backtrackingCycle(x, y, compteur, xDepart, yDepart, taille, lettres, plateau) -> bool:
    #marque la case actuelle avec son numéro de passage
    case = lettres[x] + str(y + 1)
    plateau[case] = compteur

    #condition d'arrêt
    if compteur == taille * taille:
        #verifie que la dernière case est atteignable avec un mouvement légal
        if estVoisin(x, y, xDepart, yDepart):
            return True
        else:
            #toutes les cases visitées mais pas de retour possible donc backtrack
            plateau[case] = 0
            return False

    #récupère les voisins valides et les trie par nombre de voisins
    voisins = []
    for dx, dy in mouvements:
        nx, ny = x + dx, y + dy
        if estValide(nx, ny, taille, lettres, plateau):
            voisins.append((compterVoisins(nx, ny, taille, lettres, plateau), nx, ny))
    voisins.sort()

    #teste récursivement chaque déplacement
    for nbVoisins, nx, ny in voisins:
        if backtrackingCycle(nx, ny, compteur + 1, xDepart, yDepart, taille, lettres, plateau):
            return True

    #backtrack, remet la case à 0 en cas d'impasse
    plateau[case] = 0
    return False

#affiche l'échiquier avec les numéros de passage du cavalier
def affichage_graphique(plateau, taille, lettres) -> None:
    #crée une figure avec matplotlib
    fig, ax = plt.subplots(1, 1, figsize=(taille, taille))

    #parcourt chaque case de l'échiquier
    for i in range(taille):
        for j in range(taille):
            #alterne les couleur
            if (i + j) % 2 == 0:
                couleur = "white"
            else:
                couleur = "grey"
            #dessine un carré coloré
            ax.fill([i, i+1, i+1, i], [j, j, j+1, j+1], color=couleur)

            #récupère le numéro de passage
            case = lettres[i] + str(j + 1)
            numero = plateau[case]
            #affiche le numéro de la case uniquement si visitée
            if numero > 0:
                ax.text(i + 0.5, j + 0.5, str(numero), ha="center", va="center", fontsize=12)

    #tracé des flèches

    #construit une liste de positions visitées
    ordre = []
    for i in range(taille):
        for j in range(taille):
            case = lettres[i] + str(j + 1)
            #associe chaque numéro de passage à ses coordonnées (cases visitées uniquement)
            if plateau[case] > 0:
                ordre.append((plateau[case], i + 0.5, j + 0.5))
    #trie la liste par numéro de passage croissant
    ordre.sort()

    #dessine les flèches
    for k in range(len(ordre) - 1):
        #position de départ de la flèche
        x1, y1 = ordre[k][1], ordre[k][2]
        #position d'arrivée de la flèche
        x2, y2 = ordre[k + 1][1], ordre[k + 1][2]
        #trace la flèche avec annotate
        ax.annotate("", xy=(x2, y2), xytext=(x1, y1),
                     arrowprops=dict(arrowstyle="->", color="red", lw=1.5))

    #place les lettres sur l'axe X
    ax.set_xticks([i + 0.5 for i in range(taille)])
    ax.set_xticklabels([lettres[i] for i in range(taille)])
    #place les numéros sur l'axe Y
    ax.set_yticks([j + 0.5 for j in range(taille)])
    ax.set_yticklabels([str(j + 1) for j in range(taille)])

    #limites et proportions de l'affichage
    ax.set_xlim(0, taille)
    ax.set_ylim(0, taille)
    ax.set_aspect("equal") #cases carrées
    ax.set_title("Tour du cavalier")
    plt.show() #affiche l'échiquier

#main : test chemin hamiltonien sur plateau 5x5
if __name__ == "__main__":

    print("test 5x5")
    taille5 = 5
    lettres5 = "abcde"
    plateau5 = {}
    for i in range(taille5):
        for j in range(taille5):
            plateau5[lettres5[i] + str(j + 1)] = 0

    if backtracking(0, 0, 1, taille5, lettres5, plateau5):
        print("Chemin hamiltonien trouvé sur 5x5\n")
        print(plateau5)
        affichage_graphique(plateau5, taille5, lettres5)
    else:
        print("Pas de solution trouvée\n")



    #parcours ouvert 8x8, départ h8 (coin haut-droit)
    print("parcours ouvert 8x8, départ h8")

    #recréation du plateau 8x8
    taille8 = 8
    lettres8 = "abcdefgh"
    plateau8 = {}
    for i in range(taille8):
        for j in range(taille8):
            plateau8[lettres8[i] + str(j + 1)] = 0

    #départ en h8 = coordonnées (7, 7)
    if backtracking(7, 7, 1, taille8, lettres8, plateau8):
        print("Chemin hamiltonien trouvé\n")
        print(plateau8)
        affichage_graphique(plateau8, taille8, lettres8)
    else:
        print("Pas de solution trouvée\n")


    

    #test 6x6 tour fermé, départ a1 (coin bas-gauche)
    print("test 6x6 tour fermé (départ a1)")

    #création du plateau 6x6
    taille6 = 6
    lettres6 = "abcdef"
    plateau6 = {}
    for i in range(taille6):
        for j in range(taille6):
            plateau6[lettres6[i] + str(j + 1)] = 0

    #départ en a1 = coordonnées (0, 0), recherche d'un cycle hamiltonien
    if backtrackingCycle(0, 0, 1, 0, 0, taille6, lettres6, plateau6):
        print("Cycle hamiltonien trouvé\n")
        print(plateau6)
        affichage_graphique(plateau6, taille6, lettres6)
    else:
        print("Pas de solution trouvée\n")

    #test 8x8 tour fermé, départ c5
    print(" Test 8x8 tour fermé (départ c5)")

    plateau8Cycle = {}
    for i in range(taille8):
        for j in range(taille8):
            plateau8Cycle[lettres8[i] + str(j + 1)] = 0

    if backtrackingCycle(2, 4, 1, 2, 4, taille8, lettres8, plateau8Cycle):
        print("Cycle hamiltonien trouvé\n")
        print(plateau8Cycle)
        affichage_graphique(plateau8Cycle, taille8, lettres8)
    else:
        print("Pas de solution trouvée\n")


    #test 4x4
    print("test 4x4")
    taille4 = 4
    lettres4 = "abcd"
    plateau4 = {}
    for i in range(taille4):
        for j in range(taille4):
            plateau4[lettres4[i] + str(j + 1)] = 0

    #[taille_du_meilleur_chemin, copie_du_plateau_associé]
    cheminMax4 = [0, {}]

    if backtracking(0, 0, 1, taille4, lettres4, plateau4, cheminMax4):
        print("Chemin hamiltonien trouvé sur 4x4\n")
        affichage_graphique(plateau4, taille4, lettres4)
    else:
        print(f"Pas de solution trouvée sur 4x4. Tentative maximale : {cheminMax4[0]} cases\n")
        affichage_graphique(cheminMax4[1], taille4, lettres4)

    #test 3x3
    print("test 3x3")
    taille3 = 3
    lettres3 = "abc"
    plateau3 = {}
    for i in range(taille3):
        for j in range(taille3):
            plateau3[lettres3[i] + str(j + 1)] = 0

    #[taille_du_meilleur_chemin, copie_du_plateau_associé]
    chemin_max3 = [0, {}]

    if backtracking(0, 0, 1, taille3, lettres3, plateau3, chemin_max3):
        print("Chemin hamiltonien trouvé sur 3x3\n")
        affichage_graphique(plateau3, taille3, lettres3)
    else:
        print(f"Pas de solution trouvée sur 3x3. Tentative maximale : {chemin_max3[0]} cases\n")
        affichage_graphique(chemin_max3[1], taille3, lettres3)
