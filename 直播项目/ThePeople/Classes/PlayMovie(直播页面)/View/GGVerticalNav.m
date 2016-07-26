//
//  GGVerticalNav.m
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "GGVerticalNav.h"
#import "UIButton+Button.h"
@implementation GGVerticalNav{
    UIButton *_fullScreenBtn;
    UIButton *_backBtn;
    UILabel *_peopleCount;
    BOOL _staraView;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _backBtn = [UIButton ButtonWithRect:CGRectMake(5, 5, 30, 30) title:nil titleColor:nil Image:@"movie_back_s" HighlightedImage:nil clickAction:@selector(OnClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:101];
        [self addSubview:_backBtn];
        
        _fullScreenBtn = [UIButton ButtonWithRect:CGRectMake(self.frame.size.width-35, self.frame.size.height-35, 30, 30) title:nil titleColor:nil Image:@"movie_fullscreen" HighlightedImage:nil clickAction:@selector(OnClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:102];
        [self addSubview:_fullScreenBtn];
        _staraView = NO;
    }
    return self;
}
-(void)OnClick:(UIButton *)btn{
    if ([self.delegate respondsToSelector:@selector(didWillVerticalNavOnclick:)]) {
        [self.delegate didWillVerticalNavOnclick:btn.tag];
    }
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    if (_staraView == NO) {
        _backBtn.hidden = YES;
        _fullScreenBtn.hidden = YES;
        _staraView = YES;
    }else{
        _backBtn.hidden = NO;
        _fullScreenBtn.hidden = NO;
        _staraView = NO;
    }
}
@end
