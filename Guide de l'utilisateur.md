#OZ-Slider version 1.0
#Guide de l'utilisateur
--
## Installation du matériel
### Connexion des modules
## Configuration du cycle
Une fois les éléments correctement connectés et sous tension, un message de Bienvenue est affiché.

>`Bienvenue dans`
 
>`OZ-Slider v1.0!`

### Luminosité du LCD
Les touches `Up` et `Down` permettent de modifier la luminosité de l'affichage LCD.
Cette fonction est disponible uniquement quand le message de Bienvenue est affiché.

`Select` pour passer à l'étape suivante.
### Longueur de déplacement du chariot
>`Long. du mvt.  >

>680 mm

`Right` permet de passer au paramètre suivant.

Par défaut il s'agit de la valeur maximum de déplacement du chariot en mm. Il n'est pas possible de saisir une valeur supérieure à la valeur maximum.

`Select` pour modifier la valeur. Le curseur clignote sur le premier digit.

`Right` ou `Left` pour changer de digit.

`Up` ou `Down` pour changer la valeur du digit.

`Select` pour valider la modification.
### Durée du cycle
>`< Long. cycle >`

>`60 min`

`Left` permet de revenir au paramètre précédent.

`Right` permet de passer au paramètre suivant.

Durée en minute du cycle complet de prise de vues et de déplacement. Par défaut il est à 60 min. La durée maximale est de 1020 minutes soit 17h.

`Select` pour modifier la valeur. Le curseur clignote sur le premier digit.

`Right` ou `Left` pour changer de digit.

`Up` ou `Down` pour changer la valeur du digit.

`Select` pour valider la modification.
### Intervalle de prise de vue
>`< Intervalle   >`

>`3 s`

`Left` permet de revenir au paramètre précédent.

`Right` permet de passer au paramètre suivant.

Durée d'un cycle, déplacement plus prise de vue en seconde. Par défaut il est à 3s.

`Select` pour modifier la valeur. Le curseur clignote sur le premier digit.

`Right` ou `Left` pour changer de digit.

`Up` ou `Down` pour changer la valeur du digit.

`Select` pour valider la modification.
### Direction du déplacement
>`<  Direction   >`

>`Depuis le moteur`

`Left` permet de revenir au paramètre précédent.

`Right` permet de passer au paramètre suivant.

Deux choix possibles, `Depuis le moteur` ou `Vers le moteur`. Par défaut c'est le choix `Depuis le moteur` qui est actif.

`Select` pour modifier la valeur.

`Up` ou `Down` pour changer la direction.

`Select` pour valider la modification.
### Terminer la configuration
`Left` permet de revenir au paramètre précédent.

>`< Terminer conf?`

`Select` pour terminer la configuration.

## Lancement du cycle
### Affichage de la configuration du cycle
Une fois la configuration validée, elle est affichée sur deux pages écrans.
>`Mvt. : 680 mm`

>`Cycle : 60 min`

`Down` pour afficher la page suivante.

>`Inter.: 3 s`

>`Depuis le moteur`

`Up` pour afficher la page précédente.

`Select` pour passer à l'étape suivante.
### Affichage des paramètres calculés
#### Temps de pause maximum
Le boîtier photo doit être configuré avec un temps de pause inférieur à celui indiqué. Le temps calculé tient compte du déplacement du chariot, de son amortissement et du temps de déclenchement du boitier. L'unité est la milliseconde.
>`Temps de pause `

>`2243 ms maximum`

`Select` pour passer à l'étape suivante.
#### Nombre de pauses
Cette valeur indique le nombre de photos qui seront réalisées pendant le cycle.
>`Nb de pauses`

>`900 pauses`

`Select` pour passer à l'étape suivante.
### Dernières consignes avant le lancement du cycle
#### Connecter l'appareil photo
Ne pas oublier de connecter l'appareil photo à l'aide du câble de commande.

`Select` pour passer à l'étape suivante.
#### Placer le chariot
Bien positionner le chariot au point de départ. **Attention !** à l'indication donnée par le module de commande.

* Côté moteur : Le chariot doit être positionné au plus prés du moteur.
* Opposé au moteur : Le chariot doit être positionné à l'opposé du moteur.

**Important !** Un mauvais positionnement peut entrainer un blocage et une surchauffe du moteur.

`Select` pour passer à l'étape suivante.
### Lancement du cycle ?
>`Lancement du`

>`cycle ?`

L'appuie sur `Select` lance le cycle de déplacement + photo.

Pendant le cycle sont affiché :

* Le nombre de photos restant à prendre.
* Le nombre de mm parcourus.

## Fin du cycle 
A la fin du cycle le message `Fin de cycle !` est affiché.
Le chariot reste maintenu pas le moteur.

L'appuie sur `Select` libère le chariot.

**Attention !** Si le rail n'est pas à l'horizontal le chariot peut se déplacer et provoquer la chute du matériel.

## Arrêt du cycle
Le bouton `Rst` permet à tout moment d'arrêter le cycle.

**Attention !** Les paramêtres seront perdus il faudra reprendre le cycle depuis le début.