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
}
