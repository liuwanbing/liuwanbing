//
//  GGRecomHeaderView.h
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GGRecomHeaderModel.h"
@interface GGRecomHeaderView : UICollectionReusableView
@property (weak, nonatomic) IBOutlet UILabel *name;
@property (weak, nonatomic) IBOutlet UIButton *goUp;
@property (nonatomic , strong)GGRecomHeaderModel *recomHeaderModel;
@end
