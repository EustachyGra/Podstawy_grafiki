//
//  GameScene.swift
//  mad_birds
//
//  Created by student on 16/04/2026.
//

import SpriteKit
import GameplayKit

class GameScene: SKScene, SKPhysicsContactDelegate {
    
    static var label : SKLabelNode?
    static var parrot : ParrotNode?
    override func didMove(to view: SKView) {
        
        // Get label node from scene and store it for use later
        GameScene.label = self.childNode(withName: "//helloLabel") as? SKLabelNode
        GameScene.parrot = self.childNode(withName: "//parrotNode") as? ParrotNode
        
        
        if let label = GameScene.label {
            label.alpha = 0.0
            label.run(SKAction.fadeIn(withDuration: 2.0))
            label.run(SKAction.group([
                SKAction.fadeIn(withDuration: 2.0),
                SKAction.scale(by: 0.5, duration: 2.0),
                SKAction.move(by: CGVector(dx: 0, dy: 0.5), duration: 2.0)
            ]))
        }
        self.physicsWorld.contactDelegate = self
    }
    
    func didBegin( _ contact: SKPhysicsContact)
    {
        let stoneA = contact.bodyA.node as? StoneNode
        let stoneB = contact.bodyB.node as? StoneNode
        stoneA?.damage()
        stoneB?.damage()
    }
    var prevTime: TimeInterval?
    override func update(_ currentTime: TimeInterval) {
        if let prevTime = prevTime{
            let delta = currentTime - prevTime
            GameStateMachine.shared.update(deltaTime: delta)
        }
        prevTime = currentTime
    }
    func load(level: Level) {

        let bird = ParrotNode(texture: nil, color: .clear, size: CGSize(width: 50, height: 50))
        bird.configure(textureName: level.birdTexture,
                       position: CGPoint(x: level.birdPosition.x, y: level.birdPosition.y))
        bird.name = "parrotNode"
        addChild(bird)

        for enemy in level.enemies {
            let node = StoneNode(texture: nil, color: .clear, size: CGSize(width: 50, height: 50))
            node.configure(textureName: enemy.textureName,
                           position: CGPoint(x: enemy.position.x, y: enemy.position.y))
            node.name = "enemy"
            addChild(node)

            ResultManager.shared.registerEnemy() // 🔥 WAŻNE: bo init(coder:) już się nie wykona
        }
    }

    func clearDynamicNodes() {
        for node in children {
            if node.name == "enemy" || node.name == "parrotNode" {
                node.removeFromParent()
            }
        }
    }
    func spawnBird(at position: Position, textureName: String) {
        let texture = SKTexture(imageNamed: textureName)
        let bird = SKSpriteNode(texture: texture)
        
        bird.name = "parrotNode"
        bird.userData = NSMutableDictionary()
        bird.userData?["textureName"] = textureName
        
        bird.position = CGPoint(x: position.x, y: position.y)
        addChild(bird)
    }

    func spawnEnemy(at position: Position, textureName: String) {
        let texture = SKTexture(imageNamed: textureName)
        let enemy = SKSpriteNode(texture: texture)
        
        enemy.name = "enemy"
        enemy.userData = NSMutableDictionary()
        enemy.userData?["textureName"] = textureName
        
        enemy.position = CGPoint(x: position.x, y: position.y)
        addChild(enemy)
    }
    func saveLevel(named levelName: String) -> Level {
        guard let bird = self.childNode(withName: "//parrotNode") as? SKSpriteNode else {
            fatalError("Brak ptaka")
        }

        let birdTexture = bird.userData?["textureName"] as? String ?? "ptak"

        let birdPosition = Position(
            x: Double(bird.position.x),
            y: Double(bird.position.y)
        )

        let enemyNodes = self.children.compactMap { $0 as? SKSpriteNode }
            .filter { $0.name == "enemy" }

        let enemies: [Enemy] = enemyNodes.map {
            let textureName = $0.userData?["textureName"] as? String ?? "stone_h_3"

            return Enemy(
                position: Position(
                    x: Double($0.position.x),
                    y: Double($0.position.y)
                ),
                textureName: textureName
            )
        }

        return Level(
            name: levelName,
            birdPosition: birdPosition,
            birdTexture: birdTexture,
            enemies: enemies
        )
    }
}
