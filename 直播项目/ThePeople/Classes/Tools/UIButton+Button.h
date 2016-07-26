//
//  UIButton+Button.h
//  ThePeople
//
//  Created by aoyolo on 16/4/14.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIButton (Button)
+(UIButton *)ButtonWithRect:(CGRect)rect title:(NSString *)title
                 titleColor:(UIColor *)color Image:(NSString *)image HighlightedImage:(NSString *)highlightedImage clickAction:(SEL)clickAction viewController:(id)viewController contentEdgeInsets:(UIEdgeInsets)contentEdgeInsets tag:(NSInteger)tag;
@end
