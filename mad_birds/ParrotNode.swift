import SpriteKit

class ParrotNode: SKSpriteNode {
    
    var startScenePosition: CGPoint = .zero   // pozycja z edytora
    var startLocation: CGPoint?
    var emitter : SKEmitterNode?
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        print("To ja Polly!")
        //let emmiter = self.childNode(withName: "emmiter") as? SKEmitterNode
        //emmiter?.targetNode = self.parent
        // zapamiętaj pozycję startową z .sks
        self.startScenePosition = self.position
        
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        self.physicsBody?.affectedByGravity = false
        self.startLocation = self.position
        emitter?.removeFromParent()
        emitter = nil
        
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        
        let currentLocation = touch.location(in: self.parent!)
        let previousLocation = touch.previousLocation(in: self.parent!)
        
        let newPosition = CGPoint(
            x: self.position.x + currentLocation.x - previousLocation.x,
            y: self.position.y + currentLocation.y - previousLocation.y
        )
        
        self.position = newPosition
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        
        guard let touch = touches.first else { return }
        
        self.physicsBody?.affectedByGravity = true
        
        guard let start = startLocation else { return }
        
        let currentLocation = touch.location(in: self.parent!)
        
        let vector = CGVector(
            dx: (start.x - currentLocation.x) * 6,
            dy: (start.y - currentLocation.y) * 6
        )
        if(emitter == nil){
            emitter = SKEmitterNode(fileNamed: "MyParticle.sks")
            emitter?.targetNode = self.parent
            self.addChild(emitter!)
        }
        self.physicsBody?.applyImpulse(vector)
        GameStateMachine.shared.enter(DestructionGameState.self)
    }
}
