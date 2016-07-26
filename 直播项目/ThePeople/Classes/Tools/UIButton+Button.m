//
//  UIButton+Button.m
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "UIButton+Button.h"

@implementation UIButton (Button)

+(UIButton *)ButtonWithRect:(CGRect)rect title:(NSString *)title
                 titleColor:(UIColor *)color Image:(NSString *)image HighlightedImage:(NSString *)highlightedImage clickAction:(SEL)clickAction viewController:(id)viewController contentEdgeInsets:(UIEdgeInsets)contentEdgeInsets tag:(NSInteger)tag
{
  
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = rect;
    [button setTitle:title forState:UIControlStateNormal];
    [button setImage:[UIImage imageNamed:image] forState:UIControlStateNormal];
    [button setImage:[UIImage imageNamed:highlightedImage] forState:UIControlStateHighlighted];
    [button setTitleColor:color forState:UIControlStateNormal];
    button.contentEdgeInsets=contentEdgeInsets;
    button.titleLabel.font = [UIFont systemFontOfSize:12];
    button.backgroundColor = [UIColor blackColor];
    if (clickAction) {
        [button addTarget:viewController
                   action:clickAction
         forControlEvents:UIControlEventTouchUpInside];
    }
    
    button.layer.cornerRadius = button.frame.size.width/2;
    button.layer.masksToBounds = YES;
    button.tag = tag;
    return button;
}
@end
