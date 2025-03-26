Crédits :

Modèles sur free3d.com :


# La Guerre des couleurs

This project is stategy checkered board game and rendered on CPU using GL4D library. Adverdary player is a simple AI.

## How to Play

On the current version, there is no interface for key binds. These are on the terminal.

### Rules

Each player starts with 500 gold and a castle on the middle of the checkboard. The goal is to destroy all castles from your opponent.

### Keys :

- `E` to end turn
- `C` to spawn a character, then click on one of your castles on the checkboard, then click on one of the keys below :
    - `G` for a Guerrier (they have strong attack)
    - `S` for a Seigneur (they can turn into a castle)
    - `P` for a Paysan (they produce gold)
- `M` to move a character. Click on a character on the checkboard and click on its destination. Some characters can go furher than others.
- `A` to attack an opponent. Click on the attacker on the checkboard and then click on an adjacent opponent.
- `T` to turn a Seigneur into a Castle

## Dependencies 

- `GL4Dummies` API 
- `libsdl2-dev` on Linux distros based on Debian (including Ubuntu)

## Downloading

1. Install dependencies :
```bash
git clone https://github.com/noalien/GL4Dummies
sudo apt-get install libsdl2-image-dev
```

2. Clone project repository :
```bash
git clone https://github.com/CelianNourry/Guerre-des-Couleurs
cd Guerre-des-Couleurs
```

## Credits

### Authors 
 - Farès Belhadj
 - Celian Nourry

### Models
- `castle` : AlexfreeStockVideo
- `tank` : panosdalk
- `tractor` : anupatel
- `slenderman` : lovelyassassin

