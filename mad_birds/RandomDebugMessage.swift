//
//  RandomDebugMessage.swift
//  mad_birds
//
//  Created by student on 23/04/2026.
//
class RandomDebugMessage{
    static var shared = RandomDebugMessage()
    
    func getMessage(){
        let x: Int = Int.random(in: 0...10)
        switch x{
        case 0:
            print("STONE FOR THE STONE GOD!!!")
        case 1:
            print("A crumbling deafeat")
        case 2:
            print("Boom! Geology defeated.")
        case 3:
            print("Stone-cold destruction.")
        case 4:
            print("You just rocked that!")
        case 5:
            print("Crushed it. Literally.")
        case 6:
            print("From stone to gone.")
        case 7:
            print("Just a pile of regrets now.")
        case 8:
            print("Zero rocks given.")
        case 9:
            print("A smashing success!")
        case 10:
            print("Rock and roll… into pieces.")
        default:
            break
        }
        
        
    }
}
