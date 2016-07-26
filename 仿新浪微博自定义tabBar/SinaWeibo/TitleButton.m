//
//  TitleButton.m
//  SinaWeibo
//
//  Created by lanou on 16/6/4.
//  Copyright © 2016年 xunjiangtao. All rights reserved.
//

#import "TitleButton.h"
#import "UIView+Expand.h"

@implementation TitleButton

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        self.titleLabel.font = [UIFont boldSystemFontOfSize:19];
        [self setImage:[UIImage imageNamed:@"navigationbar_arrow_down"] forState:UIControlStateNormal];
        [self setImage:[UIImage imageNamed:@"navigationbar_arrow_up"] forState:UIControlStateSelected];
    }
    return self;
}

// 目的：想在系统计算和设置完按钮的尺寸后，再修改一下尺寸
/**
 *  重写setFrame:方法的目的：拦截设置按钮尺寸的过程
 *  如果想在系统设置完控件的尺寸后，再做修改，而且要保证修改成功，一般都是在setFrame:中设置
 */
- (void)setFrame:(CGRect)frame
{
    frame.size.width += 5;
    [super setFrame:frame];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    
    // 设置按钮内部tilteLabel和imageView的位置
    
    // 1.计算titleLabel的frame
    self.titleLabel.x = self.imageView.x;
    
    // 2，计算imageView的frame
    self.imageView.x = CGRectGetMaxX(self.titleLabel.frame) + 5;
}

// 重写setTitle和setImage的方法
- (void)setTitle:(NSString *)title forState:(UIControlState)state {
    [super setTitle:title forState:state];
    
    // 只要修改了文字，就让按钮重写计算自己的尺寸
    [self sizeToFit];
}

- (void)setImage:(UIImage *)image forState:(UIControlState)state {
    [super setImage:image forState:state];
    
    [self sizeToFit];
}



@end
