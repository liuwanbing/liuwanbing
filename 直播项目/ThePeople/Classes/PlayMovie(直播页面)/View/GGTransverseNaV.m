//
//  GGTransverseNaV.m
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "GGTransverseNaV.h"
#import "UIButton+Button.h"

@implementation GGTransverseNaV{
    UIButton *_backBtn;
    UILabel *_nameLabel;
    UIButton *_definitionBtn;
    UIButton *_startBtn;
    UIButton *_reloadBtn;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _backBtn = [UIButton ButtonWithRect:CGRectMake(5, 5, 30, 30) title:nil titleColor:nil Image:@"movie_back_s" HighlightedImage:nil clickAction:@selector(OnWillClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:201];
        [self addSubview:_backBtn];
        
        _definitionBtn = [UIButton ButtonWithRect:CGRectMake(self.frame.size.width-50, 5, 30, 30) title:nil titleColor:nil Image:@"movie_setting" HighlightedImage:nil clickAction:@selector(OnWillClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:202];
        [self addSubview:_definitionBtn];
        
//        _startBtn = [UIButton ButtonWithRect:CGRectMake(5, self.frame.size.height-40, 30, 30) title:nil titleColor:nil Image:@"movie_play" HighlightedImage:nil clickAction:@selector(OnClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:203];
//        [self addSubview:_startBtn];
//        
//        _reloadBtn = [UIButton ButtonWithRect:CGRectMake(40, self.frame.size.height-40, 30, 30) title:nil titleColor:nil Image:@"movie_back_s" HighlightedImage:nil clickAction:@selector(OnClick:) viewController:self contentEdgeInsets:UIEdgeInsetsZero tag:204];
//        [self addSubview:_reloadBtn];
    }
    return self;
}
-(void)OnWillClick:(UIButton*)btn{
    if ([self.delegate respondsToSelector:@selector(TransverseNaVWillDidOnclick:)]) {
        [self.delegate TransverseNaVWillDidOnclick:btn.tag];
    }

}
@end
