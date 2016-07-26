//
//  CustomTabBar.h
//  SinaWeibo
//
//  Created by lanou on 16/6/4.
//  Copyright © 2016年 xunjiangtao. All rights reserved.
//

#import <UIKit/UIKit.h>

@class CustomTabBar;

// CustomTabBar继承自UITabBar，所以CustomTabBar的代理必须遵循UITabBar的代理协议
@protocol CustomTabBarDelegate <UITabBarDelegate>

@optional
- (void)tabBarDidClickCenterButton:(CustomTabBar *)tabBar;

@end

@interface CustomTabBar : UITabBar

@property(nonatomic, weak) id<CustomTabBarDelegate> delegate;

@end
