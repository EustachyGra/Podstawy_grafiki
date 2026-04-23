//
//  GameState.swift
//  mad_birds
//
//  Created by student on 23/04/2026.
//

import GameplayKit

class ParrotGameState: GKState{
    var time : TimeInterval = 0
    override func update(deltaTime secods: TimeInterval){
        self.time += secods
    }
    
}
class StartGameState : ParrotGameState{
    override func update(deltaTime secods: TimeInterval){
        super.update(deltaTime: secods)
        //przejdzmy do innego stanu po 5 sek.
        if (time > 5)
        {
            self.stateMachine?.enter(PlayGameState.self)
        }
    }
    override func didEnter(from previousState: GKState?)
    {
        print("Jestem w stanie Start")
        GameScene.parrot?.isUserInteractionEnabled = false
    }
}

class PlayGameState : ParrotGameState{
    override func update(deltaTime secods: TimeInterval){
        super.update(deltaTime: secods)
    }
    override func didEnter(from previousState: GKState?)
    {
        GameScene.label?.text = "Let's kill them ALL!!!!"
        GameScene.parrot?.isUserInteractionEnabled = true
    }
    override func isValidNextState(_ stateClass: AnyClass) -> Bool{
        if stateClass == DestructionGameState.self{
            return true
        }
        return false
    }
    override func willExit(to nextState: GKState)
    {
        GameScene.parrot?.isUserInteractionEnabled = false
    }
}

class DestructionGameState : ParrotGameState{
    override func update(deltaTime secods: TimeInterval){
        super.update(deltaTime: secods)
        if(time>10)
        {
            if(ResultManager.shared.isScuccess())
            {
                self.stateMachine?.enter(SuccessGameState.self)
            }
            else{
                self.stateMachine?.enter(FailureGameState.self)
            }
        }
    }
}
class SuccessGameState : ParrotGameState{
    override func update(deltaTime secods: TimeInterval){
        super.update(deltaTime: secods)
    }
    override func didEnter(from previousState: GKState?)
    {
        GameScene.label?.text = "Pokazales im na co cie stac!"
        GameScene.parrot?.isUserInteractionEnabled = false
    }
}

class FailureGameState : ParrotGameState{
    override func update(deltaTime secods: TimeInterval){
        super.update(deltaTime: secods)
    }
    override func didEnter(from previousState: GKState?)
    {
        GameScene.label?.text = "Pokonaly cie zwykle kamyczki?"
        GameScene.parrot?.isUserInteractionEnabled = false
    }
}
