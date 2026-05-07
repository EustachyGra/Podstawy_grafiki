//
//  GameStateMachine.swift
//  mad_birds
//
//  Created by student on 23/04/2026.
//

import GameplayKit

class GameStateMachine: GKStateMachine{
    static let shared = GameStateMachine()
    weak var gvc:GameViewController?
    init()
    {
        super.init(states: [
            StartGameState(),
            PlayGameState(),
            DestructionGameState(),
            SuccessGameState(),
            FailureGameState()
        ])
        self.enter(StartGameState.self)
    }
}
