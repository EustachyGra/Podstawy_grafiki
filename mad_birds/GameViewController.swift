//
//  GameViewController.swift
//  mad_birds
//
//  Created by student on 16/04/2026.
//

import UIKit
import SpriteKit
import GameplayKit
struct Position: Codable {
    let x: Double
    let y: Double
}

struct Enemy: Codable {
    let position: Position
    let textureName: String
    //let rotation : float
}

struct Level: Codable {
    let name: String
    let birdPosition: Position
    let birdTexture: String
    let enemies: [Enemy]
}
class GameViewController: UIViewController {

    func loadLevelData(level: Int) -> Level? {

        let fileName = "level\(level).json"

        // 1. Documents (zapisane poziomy)
        let documentsURL = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)[0]
            .appendingPathComponent(fileName)

        if FileManager.default.fileExists(atPath: documentsURL.path) {
            do {
                let data = try Data(contentsOf: documentsURL)
                return try JSONDecoder().decode(Level.self, from: data)
            } catch {
                print("Błąd Documents: \s(error)")
            }
        }

        print("Brak levela")
        return nil
    }
    func saveCurrentLevel(levelNumber: Int) {
        guard let view = self.view as? SKView else { return }
        guard let scene = view.scene as? GameScene else {
            print("Brak sceny")
            return
        }

        let level = scene.saveLevel(named: "Level \(levelNumber)")
        saveLevelToFile(level: level, levelNumber: levelNumber)
    }
    
    func saveLevelToFile(level: Level, levelNumber: Int) {
        let encoder = JSONEncoder()
        encoder.outputFormatting = .prettyPrinted

        do {
            let data = try encoder.encode(level)

            let url = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)[0]
                .appendingPathComponent("level\(levelNumber).json")

            try data.write(to: url)

            print("Zapisano level \(levelNumber) do: \(url)")
        } catch {
            print("Błąd zapisu: \(error)")
        }
    }
    func loadLevel(_ level: Int) {
        guard let view = self.view as? SKView else { return }

        guard let scene = SKScene(fileNamed: "GameScene") as? GameScene else {
            print("Nie udało się wczytać GameScene")
            return
        }

        guard let levelData = loadLevelData(level: level) else { return }

        scene.scaleMode = .aspectFill

        scene.clearDynamicNodes()

        scene.load(level: levelData)

        view.presentScene(scene)
    }
    override func viewDidLoad() {
        super.viewDidLoad()
        
        if let view = self.view as? SKView {
            if let scene = SKScene(fileNamed: "GameScene") as? GameScene {
                scene.scaleMode = .aspectFill
                loadLevel(1);
                
                
                //view.presentScene(scene)
                //DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                    //self.saveCurrentLevel(levelNumber: 2)
                //}
            }

            view.ignoresSiblingOrder = true
            view.showsFPS = true
            view.showsNodeCount = true
        }

        GameStateMachine.shared.gvc = self
    }

    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        if UIDevice.current.userInterfaceIdiom == .phone {
            return .allButUpsideDown
        } else {
            return .all
        }
    }

    override var prefersStatusBarHidden: Bool {
        return true
    }
}
