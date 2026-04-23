//
//  ResultManager.swift
//  mad_birds
//
//  Created by student on 23/04/2026.
//

class ResultManager{
    static let shared = ResultManager();
    private var enemies = 0;
    private var defeat = 0;
    private var isWin : Bool = false
    private init(){
        
    }
    func isScuccess() -> Bool{
        return isWin;
    }
    
    func registerEnemy()
    {
        enemies+=1
    }
    func defeatEnemy()
    {
        defeat+=1
        if(defeat>enemies/2)
        {
            isWin = true;
        }
    }
}
