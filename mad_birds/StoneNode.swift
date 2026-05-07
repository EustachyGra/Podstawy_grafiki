//
//  StoneNode.swift
//  mad_birds
//
//  Created by student on 16/04/2026.
//

import SpriteKit
class StoneNode : SKSpriteNode{
    var hp = 10;
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        ResultManager.shared.registerEnemy();
    }
    override init(texture: SKTexture?, color: UIColor, size: CGSize) {
        super.init(texture: texture, color: color, size: size)
    }
    func configure(textureName: String, position: CGPoint) {

        let texture = SKTexture(imageNamed: "stone_h_3")
        self.size = CGSize(width: 176.275, height: 42.463)
        self.texture = texture

        self.position = position

        self.physicsBody = SKPhysicsBody(texture: texture, size: self.size)
        self.physicsBody?.affectedByGravity = false
        self.physicsBody?.allowsRotation = true
        self.physicsBody?.isDynamic = true
    }
    func damage()
    {
        hp = hp-1;
        //print("Hp = \(hp)")
        if( hp == 7 )
        {
            self.texture = SKTexture(imageNamed: "stone_h_2")
        }
        if( hp == 4 )
        {
            self.texture = SKTexture(imageNamed: "stone_h_1")
        }
        if( hp == 0 )
        {
            RandomDebugMessage.shared.getMessage();
            self.removeFromParent()
            ResultManager.shared.defeatEnemy()
        }
    }
}

