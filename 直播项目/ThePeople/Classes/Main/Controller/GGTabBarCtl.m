//
//  GGTabBarCtl.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGTabBarCtl.h"
#import "GGNavCtl.h"
#import "GGRecommendVC.h"
#import "GGColumnVCtl.h"
#import "GGDirectSeedingVCtl.h"
#import "GGProfileVCtl.h"
@interface GGTabBarCtl ()

@end

@implementation GGTabBarCtl

+(void)initialize{
    //获取当前页面下所有的item
    UITabBarItem *item = [UITabBarItem appearance];
    //设置当前页面下所有item的颜色
    [item setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor orangeColor]} forState:UIControlStateSelected];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self addChildVCS];
}
#pragma mark 添加全部控制器
-(void)addChildVCS{
    //推荐
    GGRecommendVC *recommendHome = [[GGRecommendVC alloc]init];
    [self addChildVC:recommendHome andTitle:@"推荐" andImage:@"tabbar_home" andSelectedImage:@"tabbar_home_sel"];
    //栏目
    GGColumnVCtl *column = [[GGColumnVCtl alloc]init];
    [self addChildVC:column andTitle:@"栏目" andImage:@"tabbar_game" andSelectedImage:@"tabbar_game_sel"];
    //直播
    GGDirectSeedingVCtl *dir = [[GGDirectSeedingVCtl alloc]init];
    [self addChildVC:dir andTitle:@"直播" andImage:@"tabbar_room" andSelectedImage:@"tabbar_room_sel"];
    //我的
    GGProfileVCtl *my = [[GGProfileVCtl alloc]init];
    [self addChildVC:my andTitle:@"我的" andImage:@"tabbar_me" andSelectedImage:@"tabbar_me_sel"];
}
#pragma mark 添加一个控制器
- (void)addChildVC:(UIViewController *)childC andTitle:(NSString *)title andImage:(NSString *)image andSelectedImage:(NSString *)selectedImage{
    childC.title = title;
    childC.tabBarItem.image = [[UIImage imageNamed:image]imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
    childC.tabBarItem.selectedImage = [[UIImage imageNamed:selectedImage]imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
    GGNavCtl *nav = [[GGNavCtl alloc]initWithRootViewController:childC];
    [self addChildViewController:nav];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}
@end
