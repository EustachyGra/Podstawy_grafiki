import SpriteKit

class ParrotNode: SKSpriteNode {

    var startScenePosition: CGPoint = .zero
    var startLocation: CGPoint?
    var emitter: SKEmitterNode?

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        print("To ja Polly!")
        self.startScenePosition = self.position
        self.isUserInteractionEnabled = true
    }

    override init(texture: SKTexture?, color: UIColor, size: CGSize) {
        super.init(texture: texture, color: color, size: size)
        self.isUserInteractionEnabled = true
    }

    func configure(textureName: String, position: CGPoint) {

        // 🔥 FIX: używaj parametru, nie "bird"
        let texture = SKTexture(imageNamed: "ptak")
        self.size = CGSize(width: 90, height: 90)
        self.texture = texture

        self.position = position
        self.startScenePosition = position

        // 🔥 physics po ustawieniu texture
        self.physicsBody = SKPhysicsBody(texture: texture, size: self.size)
        self.physicsBody?.affectedByGravity = false
        self.physicsBody?.allowsRotation = true
        self.physicsBody?.isDynamic = true
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

        self.position = CGPoint(
            x: self.position.x + currentLocation.x - previousLocation.x,
            y: self.position.y + currentLocation.y - previousLocation.y
        )
    }

    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {

        guard let touch = touches.first else { return }

        self.physicsBody?.affectedByGravity = true

        guard let start = startLocation else { return }

        let currentLocation = touch.location(in: self.parent!)

        let vector = CGVector(
            dx: (start.x - currentLocation.x) * 3,
            dy: (start.y - currentLocation.y) * 3
        )

        if emitter == nil {
            emitter = SKEmitterNode(fileNamed: "MyParticle.sks")
            emitter?.targetNode = self.parent
            self.addChild(emitter!)
        }

        self.physicsBody?.applyImpulse(vector)

        GameStateMachine.shared.enter(DestructionGameState.self)
    }
}
