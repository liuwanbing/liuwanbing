//
//  GGPlayView.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/4/5.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGPlayView.h"

@implementation GGPlayView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
    }
    return self;
}

+(Class)layerClass{
    return [AVPlayerLayer class];
}

-(AVPlayer *)player{
    return [(AVPlayerLayer*)[self layer] player];
}

-(void)setPlayer:(AVPlayer *)player{
    [(AVPlayerLayer *)[self layer] setPlayer:player];
}
@end
