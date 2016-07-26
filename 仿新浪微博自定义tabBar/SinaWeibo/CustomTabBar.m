//
//  CustomTabBar.m
//  SinaWeibo
//
//  Created by lanou on 16/6/4.
//  Copyright © 2016年 xunjiangtao. All rights reserved.
//

#import "CustomTabBar.h"
#import "UIView+Expand.h"

@interface CustomTabBar()

@property(nonatomic, weak) UIButton *centerBtn;

@end

@implementation CustomTabBar

@dynamic delegate;

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        UIButton *centerBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        [centerBtn setBackgroundImage:[UIImage imageNamed:@"tabbar_compose_button"] forState:UIControlStateNormal];
        [centerBtn setBackgroundImage:[UIImage imageNamed:@"tabbar_compose_button_highlighted"] forState:UIControlStateHighlighted];
        [centerBtn setImage:[UIImage imageNamed:@"tabbar_compose_icon_add"] forState:UIControlStateNormal];
        [centerBtn setImage:[UIImage imageNamed:@"tabbar_compose_icon_add_highlighted"] forState:UIControlStateHighlighted];
        
        // 设置button的size和图片一样大
        centerBtn.size = centerBtn.currentBackgroundImage.size;
        
        [centerBtn addTarget:self action:@selector(centerBtnClick) forControlEvents:UIControlEventTouchUpInside];
        self.centerBtn = centerBtn;
        [self addSubview:self.centerBtn];
        
    }
    return self;
}

// 点击中间按钮的事件
- (void)centerBtnClick {
    // 让代理对象执行代理方法
    if (self.delegate != nil && [self.delegate respondsToSelector:@selector(tabBarDidClickCenterButton:)]) {
        [self.delegate tabBarDidClickCenterButton:self];
    }
}

// 重写layoutSubviews
- (void)layoutSubviews {
    [super layoutSubviews];
    
    // 设置中间按钮的位置
    CGFloat heightDiffreence = self.centerBtn.height - self.height;
    if (heightDiffreence <= 0) {
        self.centerBtn.centerX = self.width * 0.5;
        self.centerBtn.centerY = self.height * 0.5;
    }
    else {
        self.centerBtn.centerX = self.width * 0.5;
        self.centerBtn.centerY = self.height * 0.5 - heightDiffreence / 2;
    }
    
    // 设置其他tabBarButton的frame
    CGFloat tabBarButtonW = self.width / 5;
    CGFloat tabBarButtonIndex = 0;
    for (UIView *childView in self.subviews) {
        Class class = NSClassFromString(@"UITabBarButton");
        if ([childView isKindOfClass:class]) {
            childView.x = tabBarButtonIndex * tabBarButtonW;
            childView.width = tabBarButtonW;
            // 增加索引
            tabBarButtonIndex ++;
            if (tabBarButtonIndex == 2) {
                tabBarButtonIndex ++;
            }
        }
    }
    
    
}




@end
