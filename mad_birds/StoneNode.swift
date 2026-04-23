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

