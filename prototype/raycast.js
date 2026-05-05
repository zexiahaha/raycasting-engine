const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);
const WALL_STRIP_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

class Map {
  constructor() {
    this.grid = [
      [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
      [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1],
      [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
      [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    ];
  }

  render() {
    for (let i = 0; i < MAP_NUM_ROWS; i++) {
      for (let j = 0; j < MAP_NUM_COLS; j++) {
        let tileX = j * TILE_SIZE;
        let tileY = i * TILE_SIZE;
        let tileColor = this.grid[i][j] === 1 ? '#222' : '#fff';

        stroke('#222');
        fill(tileColor);
        rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
      }
    }
  }

  hasWallAt(x, y) {
    const mapGridIndexX = Math.floor(x / TILE_SIZE);
    const mapGridIndexY = Math.floor(y / TILE_SIZE);
    if (this.grid[mapGridIndexY][mapGridIndexX] === 1) {
      return true;
    }
    return false;
  }
}

class Player {
  constructor() {
    this.x = WINDOW_WIDTH / 2;
    this.y = WINDOW_HEIGHT / 2;
    this.radius = 3;
    this.turnDirection = 0; // left -1; right +1
    this.walkDirection = 0; // back -1; front +1
    this.rotationAngle = Math.PI / 2;
    this.moveSpeed = 2;
    this.rotationSpeed = 2 * (Math.PI / 180);
  }

  render() {
    noStroke();
    fill('red');
    circle(this.x, this.y, this.radius);
    // stroke('red');
    // line(
    //   this.x,
    //   this.y,
    //   this.x + Math.cos(this.rotationAngle) * 20,
    //   this.y + (Math.sin(this.rotationAngle) * 20)
    // );
  }

  update() {
    const moveDistance = this.walkDirection * this.moveSpeed;
    this.rotationAngle += this.turnDirection * this.rotationSpeed;

    let newX = this.x + Math.cos(this.rotationAngle) * moveDistance;
    let newY = this.y + Math.sin(this.rotationAngle) * moveDistance;
    const mapX = Math.floor(newX / TILE_SIZE);
    const mapY = Math.floor(newY / TILE_SIZE);
    if (grid.hasWallAt(newX, newY)) {
      player.walkDirection = 0;
      player.turnDirection = 0;
    } else {
      this.x = newX;
      this.y = newY;
    }
  }
}

class Ray {
  constructor(rayAngle) {
    this.rayAngle = normalizeAngle(rayAngle);
    this.wallHitX = 0;
    this.wallHitY = 0;
    this.distance = 0;
    this.wasHitVertical = false;
  }
  updateFacingDirection() {
    this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
    this.isRayFacingUp = !this.isRayFacingDown;

    this.isRayFacingRight =
      this.rayAngle < Math.PI * 0.5 || this.rayAngle > 1.5 * Math.PI;
    this.isRayFacingLeft = !this.isRayFacingRight;
  }
  cast(columnId) {
    let xintercept, yintercept;
    let xstep, ystep;

    // horizontal
    let foundHorzWallHit = false;
    let horzWallHitX = 0;
    let horzWallHitY = 0;

    yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += this.isRayFacingDown ? TILE_SIZE : 0;
    xintercept = player.x + (yintercept - player.y) / Math.tan(this.rayAngle);

    ystep = TILE_SIZE;
    ystep *= this.isRayFacingUp ? -1 : 1;
    xstep = TILE_SIZE / Math.tan(this.rayAngle);
    xstep *= this.isRayFacingLeft && xstep > 0 ? -1 : 1;
    xstep *= this.isRayFacingRight && xstep < 0 ? -1 : 1;

    let nextHorzTouchX = xintercept;
    let nextHorzTouchY = yintercept;

    while (
      nextHorzTouchX >= 0 &&
      nextHorzTouchX <= WINDOW_WIDTH &&
      nextHorzTouchY >= 0 &&
      nextHorzTouchY <= WINDOW_HEIGHT
    ) {
      if (
        grid.hasWallAt(
          nextHorzTouchX,
          nextHorzTouchY - (this.isRayFacingUp ? 1 : 0),
        )
      ) {
        foundHorzWallHit = true;
        horzWallHitX = nextHorzTouchX;
        horzWallHitY = nextHorzTouchY;

        break;
      } else {
        nextHorzTouchX += xstep;
        nextHorzTouchY += ystep;
      }
    }

    // vertical
    let foundVertWallHit = false;
    let vertWallHitX = 0;
    let vertWallHitY = 0;

    xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += this.isRayFacingRight ? TILE_SIZE : 0;
    yintercept = player.y + (xintercept - player.x) * Math.tan(this.rayAngle);

    xstep = TILE_SIZE;
    xstep *= this.isRayFacingLeft ? -1 : 1;
    ystep = TILE_SIZE * Math.tan(this.rayAngle);
    ystep *= this.isRayFacingUp && ystep > 0 ? -1 : 1;
    ystep *= this.isRayFacingDown && ystep < 0 ? -1 : 1;

    let nextVertTouchX = xintercept;
    let nextVertTouchY = yintercept;

    while (
      nextVertTouchX >= 0 &&
      nextVertTouchX <= WINDOW_WIDTH &&
      nextVertTouchY >= 0 &&
      nextVertTouchY <= WINDOW_HEIGHT
    ) {
      if (
        grid.hasWallAt(
          nextVertTouchX - (this.isRayFacingLeft ? 1 : 0),
          nextVertTouchY,
        )
      ) {
        foundVertWallHit = true;
        vertWallHitX = nextVertTouchX;
        vertWallHitY = nextVertTouchY;

        break;
      } else {
        nextVertTouchX += xstep;
        nextVertTouchY += ystep;
      }
    }

    let horzHitDistance = foundHorzWallHit
      ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
      : Number.MAX_VALUE;

    let vertHitDistance = foundVertWallHit
      ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
      : Number.MAX_VALUE;

    this.wallHitX =
      horzHitDistance < vertHitDistance ? horzWallHitX : vertWallHitX;
    this.wallHitY =
      horzHitDistance < vertHitDistance ? horzWallHitY : vertWallHitY;
    this.distance =
      horzHitDistance < vertHitDistance ? horzHitDistance : vertHitDistance;

    this.wasHitVertical = vertHitDistance < horzHitDistance;
  }
  render() {
    stroke('rgba(255, 0, 0, 0.3)');
    line(player.x, player.y, this.wallHitX, this.wallHitY);
  }
}

let grid = new Map();
let player = new Player();
let rays = [];

function keyPressed() {
  if (keyCode == UP_ARROW) {
    player.walkDirection = +1;
  } else if (keyCode == DOWN_ARROW) {
    player.walkDirection = -1;
  } else if (keyCode == RIGHT_ARROW) {
    player.turnDirection = +1;
  } else if (keyCode == LEFT_ARROW) {
    player.turnDirection = -1;
  }
}

function keyReleased() {
  if (keyCode == UP_ARROW) {
    player.walkDirection = 0;
  } else if (keyCode == DOWN_ARROW) {
    player.walkDirection = 0;
  } else if (keyCode == RIGHT_ARROW) {
    player.turnDirection = 0;
  } else if (keyCode == LEFT_ARROW) {
    player.turnDirection = 0;
  }
}

function castAllRays() {
  let columnId = 0;

  let rayAngle = player.rotationAngle - FOV_ANGLE / 2;

  for (let i = 0; i < NUM_RAYS; i++) {
    // for(let i = 0; i < 1; i++) {
    rays[i].rayAngle = normalizeAngle(rayAngle);
    rays[i].updateFacingDirection();
    rays[i].cast(i);

    // let ray = new Ray(rayAngle);
    // ray.cast(columnId);
    // rays.push(ray);

    rayAngle += FOV_ANGLE / NUM_RAYS;
    // columnId++;
  }
}

function normalizeAngle(angle) {
  angle = angle % (2 * Math.PI);
  if (angle < 0) {
    angle = 2 * Math.PI + angle;
  }
  return angle;
}

function distanceBetweenPoints(x1, y1, x2, y2) {
  return Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

function setup() {
  createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);
  for (let i = 0; i < NUM_RAYS; i++) {
    rays.push(new Ray(0));
  }
}

function update() {
  player.update();
  castAllRays();
}

function draw() {
  update();
  grid.render();

  for (let ray of rays) {
    ray.render();
  }
  player.render();
}
